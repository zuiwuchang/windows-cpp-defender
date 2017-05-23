#ifndef HOOKFUNCS_H_INCLUDED
#define HOOKFUNCS_H_INCLUDED
#include <windows.h>


BOOL WINAPI mySetConsoleCtrlHandler(
  PHANDLER_ROUTINE HandlerRoutine,
  BOOL ok
);
void WINAPI filterSetConsoleCtrlHandler(
  PHANDLER_ROUTINE HandlerRoutine,
  BOOL ok
);

#endif // HOOKFUNCS_H_INCLUDED
