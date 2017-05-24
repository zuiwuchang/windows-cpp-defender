#include "stdafx.h"

#include "service.h"
#include <cpp-defender/configure.h>
#include <cpp-defender/utf.hpp>
#include "process.h"

service::service():_signal(NULL)
{
    //ctor
}

service::~service()
{
    //dtor
    stop();
    wait();
    if(_signal)
    {
        CloseHandle(_signal);
    }
}

bool service::load()
{
    KING_TRACE("service::load")

    //獲取 檔案所在目錄 並作爲工作目錄
    wchar_t wbuffer[MAX_PATH];
    GetModuleFileNameW(NULL,wbuffer,MAX_PATH);
    boost::filesystem::wpath wpath(wbuffer);
    _path = wpath.filename().wstring();
    wpath.remove_filename();
    _directory = wpath.wstring();
    std::string path = utf::wchar_to_char(_directory);
    KING_DEBUG("directory : "<<path);
    _path = _directory + L"/" + _path;
    KING_DEBUG("path : "<<utf::wchar_to_char(_path));

    //加載 配置檔案
    path += "/";
    path += CONFIGURE_FILE;

    try
    {
        boost::property_tree::ptree tree;
        boost::property_tree::read_json(path,tree);

        std::string bin = tree.get<std::string>("Bin",std::string());
        boost::algorithm::trim(bin);
        if(bin.empty())
        {
            KING_FAULT("service Bin not configure")
            return false;
        }
        _bin = utf::to_utf16(bin);
        boost::filesystem::wpath bin_path(_bin);
        if(!bin_path.is_complete())
        {
            _bin = _directory + L"/" + _bin;
        }

		std::string params = tree.get<std::string>("Params",std::string());
        boost::algorithm::trim(params);
        if(!params.empty())
        {
			_params = L" " + utf::to_utf16(params);
        }

        std::string name = tree.get<std::string>("Name",std::string());
        boost::algorithm::trim(name);
        if(name.empty())
        {
            KING_FAULT("service Name not configure")
            return false;
        }
        _name = utf::to_utf16(name);

        std::string show = tree.get<std::string>("Show",std::string());
        boost::algorithm::trim(show);
        if(show.empty())
        {
            _show = _name;
        }
        else
        {
            _show = utf::to_utf16(show);
        }

        std::string description = tree.get<std::string>("Description",std::string());
        boost::algorithm::trim(description);
        if(description.empty())
        {
            _description = _show;
        }
        else
        {
            _description = utf::to_utf16(description);
        }

        _auto = tree.get<bool>("Auto",true);

        KING_INFO("service name : "<<utf::wchar_to_char(_name))
        KING_INFO("service show : "<<utf::wchar_to_char(_show))
        KING_INFO("service desc : "<<utf::wchar_to_char(_description))
        KING_INFO("service auto : "<<_auto)

		KING_INFO("service bin    : "<<utf::wchar_to_char(_bin))
		KING_INFO("service params :"<<utf::wchar_to_char(_params))
        return true;
    }
    catch(const boost::property_tree::file_parser_error& e)
    {
        KING_FAULT(e.what())
    }
    catch(const boost::property_tree::ptree_bad_path& e)
    {
        KING_FAULT(e.what())
    }
    catch(const boost::property_tree::ptree_bad_data& e)
    {
        KING_FAULT(e.what())
    }
    return false;
}
int service::install()
{
    int result = 0;
    //打開 scm
    SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
    if(scm)
    {
        DWORD start = SERVICE_DEMAND_START;
        if(_auto)
        {
            start = SERVICE_AUTO_START;
        }

        //創建服務
        SC_HANDLE s = CreateService(scm,
                                    _name.c_str(),
                                    _show.c_str(),
                                    SERVICE_ALL_ACCESS,
                                    SERVICE_WIN32_OWN_PROCESS,	//exe中只提供單個服務
                                    start,		//手動啓動
                                    SERVICE_ERROR_NORMAL,
                                    (_path + L" -s").c_str(),
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL);

        if(s)
        {
            //修改服務描述
            SERVICE_DESCRIPTION description;
            description.lpDescription = (LPWSTR)_description.c_str();
            if(!ChangeServiceConfig2(s,SERVICE_CONFIG_DESCRIPTION,&description))
            {
                KING_ERROR("ChangeServiceConfig2 error")
            }
            CloseServiceHandle(s);
        }
        else
        {
            result = 1;
            if(ERROR_SERVICE_EXISTS == GetLastError())
            {
                KING_FAULT("Service is already exists")
            }
            else
            {
                KING_FAULT("CreateService error")
            }
        }

        //關閉 scm
        CloseServiceHandle(scm);
    }
    else
    {
        KING_FAULT("OpenSCManager error")
        result = 1;
    }

    return result;
}
int service::uninstall()
{
    int result = 0;
    //打開 scm
    SC_HANDLE scm = OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE);
    if(scm)
    {
        //打開 服務
        SC_HANDLE s = OpenService(scm,_name.c_str(),SERVICE_ALL_ACCESS);
        if(s)
        {
            if(!DeleteService(s))
            {
                result = 1;
                KING_FAULT("DeleteService error");
            }
            //關閉 服務句柄
            CloseServiceHandle(s);
        }
        else
        {
            KING_FAULT("can not OpenService "<<utf::wchar_to_char(_name))
            result = 1;
        }
        CloseServiceHandle(scm);
    }
    else
    {
        KING_FAULT("OpenSCManager error")
        result = 1;
    }
    return result;
}

//初始化 資源
bool service::init()
{
    std::wstring name = EVENT_MAGIC_NAME + _name;
    KING_INFO("signal name : "<<utf::wchar_to_char(name));
    _signal = CreateEvent(NULL,TRUE,FALSE,name.c_str());
    if(!_signal)
    {
        KING_FAULT("create signal error")
        return false;
    }
    if(GetLastError( ) == ERROR_ALREADY_EXISTS)
    {
        KING_FAULT("signal is already exists error")
        CloseHandle(_signal);
        _signal = NULL;
        return false;
    }

    return true;
}

//運行 服務
bool service::run()
{
    try
    {
        _thread = boost::make_shared<boost::thread>(boost::bind(work_thread,_directory,_bin,_params));
        return true;
    }
    catch(...)
    {
        KING_FAULT("create service thread error")
    }
    return false;
}


void service::wait()
{
    if(_thread)
    {
        _thread->join();
    }
}
void service::stop()
{
    //通知工作線程 退出
    if(_thread)
    {
        _thread->interrupt();
    }

    //通知 進程 退出
    if(_signal)
    {
        SetEvent(_signal);
    }
}
