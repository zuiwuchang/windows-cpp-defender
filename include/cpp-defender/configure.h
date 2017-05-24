#ifndef CONFIGURE_H_INCLUDED
#define CONFIGURE_H_INCLUDED


#ifdef _DEBUG
#define KING_DEBUG_USE_DEBUG
#else
#define KING_DEBUG_USE_RELEASE
#endif // _DEBUG
#include <king/Debug.hpp>


#define CONFIGURE_FILE  "my.json"

#define VERSION L"1.0.0"
#define EVENT_MAGIC_NAME L"_magic-king-cpp-defender-v1."
#endif // CONFIGURE_H_INCLUDED
