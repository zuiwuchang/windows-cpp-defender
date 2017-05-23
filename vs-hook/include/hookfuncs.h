#ifndef HOOKFUNCS_H_INCLUDED
#define HOOKFUNCS_H_INCLUDED
#include <windows.h>
BOOL WINAPI mySetConsoleCtrlHandler(
  PHANDLER_ROUTINE HandlerRoutine,
  BOOL Add
);


#endif // HOOKFUNCS_H_INCLUDED
