#pragma once

#ifdef __ANDROID__

#include <utility>
#include "android/log.h"


namespace uPacketDivideation
{

template <class T>
void DebugLog(const T &str)
{
    __android_log_print(ANDROID_LOG_DEBUG, "uPacketDivideation", "%s", str);
}

template <class ... Args>
void DebugLog(const Args &... args)
{
    __android_log_print(ANDROID_LOG_DEBUG, "uPacketDivideation", args...);
}

}

#else

namespace uPacketDivision
{

void DebugLog(const char *format, ...);

}

#endif
