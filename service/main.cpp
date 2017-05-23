#include <iostream>
#include <boost/program_options.hpp>

#include "configure.h"
#include "main.h"
#include "service.h"

int main(int argc,char* argv[])
{
    //實例化 選項描述器
    boost::program_options::options_description opts("demo options");

    //定義 要解析的 參數
    opts.add_options()
    ("help,h","show params help")
    ("service,s", "run as service")
    ("install,i","install service")
    ("uninstall,u","uninstall service")
    ;
    //解析參數
    boost::program_options::variables_map params;
    boost::program_options::store(boost::program_options::parse_command_line(argc,argv,opts),params);

    //顯示幫助
    if(params.count("help"))
    {
        std::cout<<opts<<"\n";
        return 0;
    }

    //加載配置
    service& s = service::get_mutable_instance();
    if(!s.load())
    {
        return 1;
    }

    //執行命令
    if(params.count("install"))
    {
        KING_TRACE("install service")
        return s.install();
    }
    else if(params.count("uninstall"))
    {
        KING_TRACE("uninstall service")
        return s.uninstall();
    }


    //運行 服務
    if(params.count("service"))
    {
        KING_TRACE("run as service")
        return service_main();
    }
    else
    {
        KING_TRACE("run normal")
        return normal_main();
    }
    return 0;
}
