#pragma once

#ifdef PC

#include "CorePC.h"

#endif

#if defined(IOS) || defined(OSX)

#include "CoreIOS.h"

#endif

#if defined(ANDROID)

#include "CoreAndroid.h"

#endif

#if defined(OSUNIX)

#include "CoreUnix.h"

#endif