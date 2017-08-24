#include "stdafx.h"
#include "main.h"
#include "service.h"

#include <cpp-defender/configure.h>

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) ;
int normal_main()
{
    //註冊 ctrl 鉤子
    if(!SetConsoleCtrlHandler(  CtrlHandler, TRUE ))
    {
        KING_ERROR("normal_main can not SetConsoleCtrlHandler")
    }

    //初始化
    service& s = service::get_mutable_instance();
    if(! s.init())
    {
        return 1;
    }

    //運行服務
    if(! s.run())
    {
        return 1;
    }
#ifdef _DEBUG
    //cmd
    std::string cmd;
    while(true)
    {
        std::cout<<"\n\n$>";
        std::cin>>cmd;
        if(!std::cin)
        {
            break;
        }
        if("exit" == cmd)
        {
            break;
        }
    }

    //停止 服務
    s.stop();
	s.wait();
#else
    //等待 服務結束
    s.wait();
#endif
    KING_INFO("normal_main exit success");
	std::system("pause");
    return 0;
}
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    KING_INFO("CtrlHandler "<<fdwCtrlType)
    switch( fdwCtrlType )
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        {
            service& s = service::get_mutable_instance();
            s.stop();
            s.wait();
        }
    }
    return FALSE;
}
