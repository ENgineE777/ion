
#pragma once

#include "Common/Common.h"

#ifdef EDITOR

DEF_EXPORT int RunEditor(HINSTANCE hInstance, int nArgs, char** args);
DEF_EXPORT int RunExport(int nArgs, char** args);
DEF_EXPORT int RunENgine(HINSTANCE hInstance, int nArgs, char** args);

#endif