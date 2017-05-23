#include "hook.h"
#include "hookfuncs.h"

#include "hook.hpp"
#include "service.h"
#include "../../service/configure.h"

hacker::hook_code_ptr_t g_hpSetConsoleCtrlHandler;
DWORD g_jmpSetConsoleCtrlHandler;
void init_hook()
{
    g_hpSetConsoleCtrlHandler = hacker::hook::inline_hook((DWORD)SetConsoleCtrlHandler,(DWORD)mySetConsoleCtrlHandler,5);
	if(g_hpSetConsoleCtrlHandler)
	{
		g_jmpSetConsoleCtrlHandler = g_hpSetConsoleCtrlHandler->jump;
		KING_INFO("hook SetConsoleCtrlHandler success")
	}
}

__declspec(naked)
BOOL WINAPI mySetConsoleCtrlHandler(
  PHANDLER_ROUTINE HandlerRoutine,
  BOOL ok
)
{
	__asm
	{
		push ebp
		mov ebp,esp

		push [ebp+0x0C]
		push [ebp+0x8]
		call filterSetConsoleCtrlHandler

		pop ebp


		//轉到 原始 原代碼
		mov     edi, edi
		push    ebp
		mov     ebp, esp
		jmp g_jmpSetConsoleCtrlHandler
	}
}
void WINAPI filterSetConsoleCtrlHandler(
  PHANDLER_ROUTINE HandlerRoutine,
  BOOL ok
)
{
	if(ok && HandlerRoutine)
	{
		service::get_mutable_instance().ctrl_handler(HandlerRoutine);
	}
	if(!ok)
	{
		service::get_mutable_instance().reset_ctrl_handler(HandlerRoutine);
	}
}