#include <windows.h>
#include "main.h"
#include "service.h"
VOID WINAPI ServiceMain(
    DWORD dwArgc,
    LPTSTR* lpszArgv
);
VOID WINAPI ServiceControlHandler(
    DWORD fdwControl
);
SERVICE_STATUS g_status;
SERVICE_STATUS_HANDLE g_hServer;
int service_main()
{
    //獲取服務名
    wchar_t name[MAX_PATH];
    wcscpy(name,service::get_const_instance().name().c_str());
    //定義待 註冊服務 數組
    //每個服務 會被運行在一個新啓的 線程中
    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        {
            name,	//服務名
            ServiceMain	//服務 入口點函數
        },
        {NULL,NULL} // 以空數組 代表結束
    };

    //註冊服務 並等待服務停止
    StartServiceCtrlDispatcher(serviceTable);
    return 0;
}
VOID WINAPI ServiceMain(
    DWORD dwArgc,
    LPTSTR* lpszArgv
)
{
    //獲取服務名
    wchar_t name[MAX_PATH];
    wcscpy(name,service::get_const_instance().name().c_str());

    //註冊 ControlHandler
    //返回的 句柄 不需要關閉
    g_hServer = RegisterServiceCtrlHandler(name,//服務名 必須和 SERVICE_TABLE_ENTRY中一致
                                           ServiceControlHandler	//控制回調 在 StartServiceCtrlDispatcher 線程中被回調
                                          );
    if(!g_hServer)
    {
        //error
        return;
    }
    //通知 scm 已經開始 繼續 初始化 服務
    g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_status.dwCurrentState = SERVICE_START_PENDING;	//設置當前服務 狀態
    g_status.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;	//設置允許的 ControlHandler
    g_status.dwWin32ExitCode = 0;
    g_status.dwServiceSpecificExitCode = 0;
    g_status.dwCheckPoint   = 0;
    g_status.dwWaitHint   = 0;
    SetServiceStatus(g_hServer,&g_status);

    //初始化 操作
    service& s = service::get_mutable_instance();
    if(! s.init() ||  !s.run()) //初始化 並運行
    {
        //初始化 失敗 停止服務
        g_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR ;
        g_status.dwServiceSpecificExitCode  = 1;
        g_status.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(g_hServer,&g_status);
        return ;
    }

    //通知 scm 服務 初始化 完成 正在運行
    g_status.dwWaitHint = 0;
    g_status.dwCheckPoint = 0;
    g_status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_hServer,&g_status);

    //等待服務結束
    s.wait();
}
VOID WINAPI ServiceControlHandler(
    DWORD fdwControl
)
{
    switch(fdwControl)
    {
    case SERVICE_CONTROL_STOP:	//停止 服務
    case SERVICE_CONTROL_SHUTDOWN:
        //通知 scm 已經 得到 停止通知
        g_status.dwWaitHint = 1000 * 6;	//設置預計 完成 需要時間
        g_status.dwCheckPoint = 0;		//更新 執行 進度
        g_status.dwCurrentState = SERVICE_STOP_PENDING;	//設置當前服務 狀態

        SetServiceStatus(g_hServer,&g_status);


        //執行關閉操作
        service::get_mutable_instance().stop();
        //等待服務結束
        service::get_mutable_instance().wait();

        //通知 scm 已經 停止服務
        g_status.dwWaitHint = 0;
        g_status.dwCheckPoint = 0;
        g_status.dwCurrentState = SERVICE_STOPPED;	//設置當前服務 狀態
        SetServiceStatus(g_hServer,&g_status);
        return;
    }

    SetServiceStatus(g_hServer,&g_status);
}

