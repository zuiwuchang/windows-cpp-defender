#ifndef SERVICE_H
#define SERVICE_H
#include "../../service/singleton.hpp"
#include <windows.h>
#include <boost/thread.hpp>
class service
    :public singleton<service>
{
    public:
        service();
        virtual ~service();

    protected:

    private:
            //關閉信號
        HANDLE _signal;
        //ctl 處理函數
        PHANDLER_ROUTINE _ctrlHandler;
        //_ctrlHandler 同步
        boost::mutex _mutex;
    public:
        inline void ctrl_handler(PHANDLER_ROUTINE ctrlHandler)
        {
            boost::mutex::scoped_lock  lock(_mutex);
            _ctrlHandler = ctrlHandler;
        }
        inline PHANDLER_ROUTINE ctrl_handler()
        {
            boost::mutex::scoped_lock  lock(_mutex);
            return _ctrlHandler ;
        }
		inline void reset_ctrl_handler(const PHANDLER_ROUTINE ctrlHandler)
		{
			boost::mutex::scoped_lock  lock(_mutex);
            if(ctrlHandler == _ctrlHandler)
			{
				_ctrlHandler = NULL;
			}
		}
    public:
        //開始 服務初始化 之後運行
        void run(HINSTANCE hinstDLL);
    protected:
        //工作線程
        void work_thread();

};

#endif // SERVICE_H
