#include "stdafx.h"
#include "process.h"
#include <cpp-defender/configure.h>

#define DLL_FILE L"hook.dll"
#include <cpp-defender/utf.hpp>

bool inject(HANDLE hProcess,const std::wstring& dll);
void work_thread(const std::wstring& directory,const std::wstring& bin,const std::wstring& params)
{
    try
    {
        std::wstring dll(directory + L"\\" + DLL_FILE);

        boost::filesystem::wpath path(bin);
        std::wstring dir = path.remove_filename().wstring();
        PROCESS_INFORMATION process;
        STARTUPINFO startupinfo;

		wchar_t binBuffer[MAX_PATH];
		wcscpy(binBuffer,bin.c_str());
		wchar_t binParams[MAX_PATH];
		wchar_t* paramsPtr = NULL;
		if(!params.empty())
		{
			paramsPtr = binParams;
			wcscpy(binParams,params.c_str());
		}

        while(true)
        {
            //服務停止 信號
            boost::this_thread::interruption_point();

            //創建進程
            memset(&startupinfo,0,sizeof(STARTUPINFO));
            startupinfo.cb=sizeof(STARTUPINFO);
            if(!CreateProcess(binBuffer,
                              paramsPtr,
                              NULL,
                              NULL,
                              FALSE,
                              CREATE_SUSPENDED,
                              NULL,
                              dir.c_str(),
                              &startupinfo,
                              &process))
            {
                KING_ERROR("can not create process "<<utf::wchar_to_char(bin))
                boost::this_thread::sleep(boost::posix_time::seconds(1));
                continue;
            }

            //注入 dll
            if(! inject(process.hProcess,dll))
            {
                CloseHandle(process.hProcess);
                CloseHandle(process.hThread);
                boost::this_thread::sleep(boost::posix_time::seconds(1));
                continue;
            }
            //等待 進程 執行 hook
            Sleep(2000);

            //運行 進程
			ResumeThread(process.hThread);

			//等待進程結束
			WaitForSingleObject(process.hProcess,INFINITE);
			CloseHandle(process.hProcess);
			CloseHandle(process.hThread);

			boost::this_thread::sleep(boost::posix_time::seconds(1));
        }
    }
    catch(boost::thread_interrupted&)
    {
        //停止服務
        KING_DEBUG("exit service thread")
    }
}
bool inject(HANDLE hProcess,const std::wstring& dll)
{
    //申請 遠程 內存
    int size  = (dll.size()+1)* sizeof(wchar_t);
    LPVOID buffer = VirtualAllocEx(hProcess, NULL, size, MEM_COMMIT, PAGE_READWRITE);
    if(!buffer)
    {
        KING_ERROR("VirtualAllocEx error")
        return false;
    }

    //寫入 數據
    if(!WriteProcessMemory(hProcess, buffer, dll.c_str(), size, NULL))
    {
        KING_ERROR("WriteProcessMemory error")
        return false;
    }

	//獲取 loadlibrary 地址
	LPTHREAD_START_ROUTINE func_addr = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryW");
	if ( !func_addr )
	{
		KING_ERROR("can not get LoadLibraryW address")
		return false;
	}

	DWORD id = 0;
	//遠程線程 注入
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, func_addr, buffer, 0, &id);
	if ( !hRemoteThread )
	{
        KING_ERROR("CreateRemoteThread error")
		return false;
	}
	CloseHandle(hRemoteThread);
	return true;
}
