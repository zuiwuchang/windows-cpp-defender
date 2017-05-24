#include "stdafx.h"
#include "service.h"
#include <cpp-defender/configure.h>
#include <cpp-defender/utf.hpp>
#include "hook.h"
service::service():_signal(NULL),_ctrlHandler(NULL)
{
    //ctor
}

service::~service()
{
    //dtor
    if(_signal)
    {
        CloseHandle(_signal);
    }
}

void service::run(HINSTANCE hinstDLL)
{
    //獲取dll 路徑
    wchar_t wbuffer[MAX_PATH];
    GetModuleFileName(hinstDLL,wbuffer,MAX_PATH);
    KING_TRACE("dll path : "<<utf::wchar_to_char(wbuffer));

    //獲取 配置檔案 路徑
    boost::filesystem::wpath wpath(wbuffer);
    std::string path =  utf::wchar_to_char(wpath.remove_filename().wstring());
    path += "/";
    path += CONFIGURE_FILE;
    KING_TRACE("cnf path : "<<path)

    //加載配置
    try
    {
        boost::property_tree::ptree tree;
        boost::property_tree::read_json(path,tree);

        //獲取 服務名
        std::string name = tree.get<std::string>("Name");
        boost::algorithm::trim(name);
        if(name.empty())
        {
            KING_FAULT("service Name not configure")
            ExitProcess(1);
            return;
        }

        //獲取 信號 名
        std::wstring signalName = EVENT_MAGIC_NAME + utf::to_utf16(name);
        KING_DEBUG("signal name : "<<utf::wchar_to_char(signalName));

        //打開信號 句柄
        _signal = CreateEvent(NULL,TRUE,FALSE,signalName.c_str());
        if(!_signal)
        {
            KING_ERROR("can not get  signal Handle"<<utf::wchar_to_char(signalName))
        }

        //執行 hook 操作
        init_hook();

        //運行 工作線程
        boost::thread thread(boost::bind(&service::work_thread,this));
        return;
    }
    catch(const boost::property_tree::file_parser_error& e)
    {
        KING_FAULT(e.what());
    }
    catch(const boost::property_tree::ptree_bad_path& e)
    {
        KING_FAULT(e.what());
    }
    catch(const boost::property_tree::ptree_bad_data& e)
    {
        KING_FAULT(e.what());
    }
    //初始化 異常 退出進程
    ExitProcess(1);
}
void service::work_thread()
{
    //等待 結束 信號
	KING_INFO("wait ctrl + c signal")
	WaitForSingleObject(_signal,INFINITE);

	//調用 ctrl + c
	PHANDLER_ROUTINE ctrlHandler = ctrl_handler();
	
	if(ctrlHandler)
	{
		ctrlHandler(CTRL_C_EVENT);
	}
	
	//關閉 進程
	ExitProcess(0);
}
