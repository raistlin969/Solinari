#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLEW_STATIC

#include <Windows.h>
#include <WindowsX.h>

#include <crtdbg.h>

//c runtime headers
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <MMSystem.h>
//stl
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <map>

#include "TinyXML/tinyxml2.h"

using std::tr1::shared_ptr;
using std::tr1::weak_ptr;
using std::tr1::static_pointer_cast;
using std::tr1::dynamic_pointer_cast;

class SOL_noncopyable
{
private:
  SOL_noncopyable(const SOL_noncopyable& x);
  SOL_noncopyable& operator=(const SOL_noncopyable& x);
public:
  SOL_noncopyable() {};
};

#if defined(_DEBUG)
#define SOL_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define SOL_NEW new
#endif

#include "Core/Interfaces.h"
extern INT WINAPI EngineEntry(HINSTANCE hInstance,
                              HINSTANCE hPrevInstance,
                              LPWSTR lpCmdLine,
                              int nCmdShow);

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;