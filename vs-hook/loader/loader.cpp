// loader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) 
{ 
	return FALSE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::string cmd;
	HMODULE h = NULL;
	while(true)
	{
		std::cout<<"\n$>";
		std::cin>>cmd;
		if(cmd == "exit")
		{
			break;
		}
		else if (cmd == "load")
		{
			h = LoadLibrary(L"hook.dll");
			if(!h)
			{
				puts("load error");
			}
		}
		else if(cmd == "set")
		{
			if(!SetConsoleCtrlHandler(  CtrlHandler, TRUE ) ) 
			{
				puts("set error");
			}
		}
	}
	return 0;
}

