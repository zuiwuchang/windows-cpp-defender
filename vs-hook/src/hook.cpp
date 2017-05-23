#include "hook.h"
#include "hookfuncs.h"

#include "hook.hpp"
void init_hook()
{
    hacker::hook_code_ptr_t ptr = hacker::hook::inline_hook((DWORD)SetConsoleCtrlHandler,(DWORD)mySetConsoleCtrlHandler,5);

}

BOOL WINAPI mySetConsoleCtrlHandler(
  PHANDLER_ROUTINE HandlerRoutine,
  BOOL Add
)
{
    return true;
}
