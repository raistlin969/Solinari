#include "EngineStd.h"
#include "CoreApp.h"

INT WINAPI EngineEntry(HINSTANCE hInstance,
                              HINSTANCE hPrevInstance,
                              LPWSTR lpCmdLine,
                              int nCmdShow)
{
  //setup checks for memory leaks
  int temp_debug_flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

  //set this flag to keep memory blocks around
  //temp_debug_flag |= _CRTDBG_DELAY_FREE_MEM_DF; //will cause intermitent pauses

  //perform memory check for each alloc/dealloc
  //temp_debug_flag |= _CRTDBG_CHECK_ALWAYS_DF; //very slow

  temp_debug_flag |= _CRTDBG_LEAK_CHECK_DF;

  _CrtSetDbgFlag(temp_debug_flag);

  //init Logging

  //perfrom app init
  if(!the_app_pointer->InitInstance(hInstance, lpCmdLine, 0))
    return FALSE;
  //Mainloop
  MSG msg;

  do
  {
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
    }
  }while(msg.message != WM_QUIT);
  //shutdown

  //destroy logger

  return 0; //return app exit code
}