#include <windows.h>
#include <boost/thread.hpp>
#include <fstream>

//重新定位 日誌 輸出
class debug_out
    :public std::ostream
{
protected:
    std::ofstream outf;
public:
    debug_out()
    {
        outf.open("my.log",std::ios::out|std::ios::trunc);
    }

    //操控器 std::endl ...
    virtual  __ostream_type&
    operator<<(__ostream_type& (*__pf)(__ostream_type&))
    {
        __pf(std::cout);
        __pf(outf);
        return *this;
    }

    //基本 型別 輸出
    virtual debug_out& operator<<(const char* val)
    {
        std::cout<<val;
        outf<<val;
        return *this;
    }
    virtual debug_out& operator<<(const int val)
    {
        std::cout<<val;
        outf<<val;
        return *this;
    }
};
debug_out g_out;
#define KING_DEBUG_STREAM g_out
#ifdef _DEBUG
#define KING_DEBUG_USE_DEBUG
#else
#define KING_DEBUG_USE_RELEASE
#endif // _DEBUG
#include <king/Debug.hpp>

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

        KING_INFO("SetConsoleCtrlHandler success")
        boost::thread t(work_thread);
        g_thread = &t;
        t.join();
        KING_INFO("exit ok")
    }
    else
    {
        KING_FAULT("SetConsoleCtrlHandler error")
        return 1;
    }
    return 0;
}
void work_thread()
{
    KING_TRACE("work_thread start")
    try
    {
        while(true)
        {
            boost::this_thread::sleep(boost::posix_time::seconds(1));
            KING_INFO("at work")
        }
    }
    catch(boost::thread_interrupted&)
    {
        std::cout<<"thread is end"<<std::endl;
    }
    KING_TRACE("work_thread stop")
}
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch( fdwCtrlType )
    {
    case CTRL_C_EVENT:
        KING_DEBUG("CTRL_C_EVENT")
        if(g_thread)
        {
            g_thread->interrupt();
            g_thread->join();
            return TRUE;
        }
        return FALSE;
    case CTRL_CLOSE_EVENT:
        KING_DEBUG("CTRL_CLOSE_EVENT")
        if(g_thread)
        {
            g_thread->interrupt();
            g_thread->join();
        }
        return FALSE;
    case CTRL_LOGOFF_EVENT:
        KING_DEBUG("CTRL_LOGOFF_EVENT")
        if(g_thread)
        {
            g_thread->interrupt();
            g_thread->join();
        }
        return FALSE;
    case CTRL_SHUTDOWN_EVENT:
        KING_DEBUG("CTRL_SHUTDOWN_EVENT")
        if(g_thread)
        {
            g_thread->interrupt();
            g_thread->join();
        }
        return FALSE;
    }
    return FALSE;
}
