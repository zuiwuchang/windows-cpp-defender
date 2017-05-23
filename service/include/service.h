#ifndef SERVICE_H
#define SERVICE_H
#include "../singleton.hpp"
#include <string>
#include <boost/thread.hpp>

#include <windows.h>
//主要功能 服務 單例
class service
    :public singleton<service>
{
    public:
        service();
        virtual ~service();

    private:
        //工作目錄
        std::wstring _directory;
        //本檔案路徑
        std::wstring _path;
        //服務名稱
        std::wstring _name;
        //顯示名稱
        std::wstring _show;
        //服務描述
        std::wstring _description;
        //自動 啓動
        bool _auto;
        //被守衛進程 執行參數
        std::wstring _bin;

        //關閉信號
        HANDLE _signal;
    public:
        //加載配置
        bool load();
        //安裝服務
        int install();
        //卸載服務
        int uninstall();

        //初始化 資源
        bool init();

        //運行 服務
        bool run();
    protected:
        typedef boost::shared_ptr<boost::thread> thread_spt;
        thread_spt _thread;
    public:
        //等待服務 停止
        void wait();

        //停止服務
        void stop();
};

#endif // SERVICE_H
