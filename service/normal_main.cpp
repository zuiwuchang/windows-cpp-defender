#include "main.h"
#include "service.h"
#include <iostream>
#include "configure.h"
int normal_main()
{
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

   //cmd
   std::string cmd;
   while(true)
   {
       std::cout<<"\n\n$>";
        std::cin>>cmd;
        if("exit" == cmd)
        {
            break;
        }
   }

    //停止 服務
    s.stop();

    //等待 服務結束
    s.wait();

    KING_INFO("normal_main exit success")
    return 0;
}
