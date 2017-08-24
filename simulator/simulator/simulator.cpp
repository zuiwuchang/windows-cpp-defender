// simulator.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define KG_DEBUG_USE_DEBUG
//自定義 日誌輸出位置
std::ofstream outf("simulator.log",std::ios::out|std::ios::trunc);
class out_t
{
public:
	//實現輸出
	template<typename T>
	out_t& operator<<(const T& t)
	{
		std::cout<<t;
		outf<<t;
		return *this;
	}

	//實現刷新緩衝區
	void flush()
	{
		std::cout.flush();
	}
};
out_t _kg_logout;
#define KG_DEBUG_STREAM   _kg_logout
#include <kg/debug.hpp>
//ctrl 處理器
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) ;

//模擬 工作 線程
boost::thread* g_thread = NULL;
void work_thread();

int main()
{
    std::ofstream out;
    out<<std::endl;
    out.flush();
    //註冊 ctrl 處理器
    if(SetConsoleCtrlHandler(CtrlHandler, TRUE ) )
    {

        KG_INFO("SetConsoleCtrlHandler success")
        boost::thread t(work_thread);
        g_thread = &t;
        t.join();
        KG_INFO("exit ok")
    }
    else
    {
        KG_INFO("SetConsoleCtrlHandler error")
        return 1;
    }
    return 0;
}
void work_thread()
{
    KG_TRACE("work_thread start")
    try
    {
        while(true)
        {
            boost::this_thread::sleep(boost::posix_time::seconds(1));
            KG_INFO("at work")
        }
    }
    catch(boost::thread_interrupted&)
    {
        std::cout<<"thread is end"<<std::endl;
    }
    KG_TRACE("work_thread stop")
}
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    KG_INFO("fdwCtrlType = "<<fdwCtrlType)
    switch( fdwCtrlType )
    {
    case CTRL_C_EVENT:
        KG_INFO("CTRL_C_EVENT")
        if(g_thread)
        {
            g_thread->interrupt();
            g_thread->join();
            return TRUE;
        }
        return FALSE;
    case CTRL_CLOSE_EVENT:
        KG_INFO("CTRL_CLOSE_EVENT")
        if(g_thread)
        {
            g_thread->interrupt();
            g_thread->join();
        }
        return FALSE;
    case CTRL_LOGOFF_EVENT:
        KG_INFO("CTRL_LOGOFF_EVENT")
        if(g_thread)
        {
            g_thread->interrupt();
            g_thread->join();
        }
        return FALSE;
    case CTRL_SHUTDOWN_EVENT:
        KG_INFO("CTRL_SHUTDOWN_EVENT")
        if(g_thread)
        {
            g_thread->interrupt();
            g_thread->join();
        }
        return FALSE;
    }
    return FALSE;
}

