#include "SolinariStd.h"
#include "SolinariApp.h"

SolinariApp the_app;

INT WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPWSTR lpCmdLine,
                    int nCmdShow)
{
  return EngineEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}