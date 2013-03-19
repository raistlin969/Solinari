#pragma once

#include <Windows.h>
class GLAppWindow;

class CoreApp
{
protected:
  HINSTANCE _hinstance;
  bool _windowed;
  bool _running;
  bool _quit_requested;
  bool _quitting;
  
  GLAppWindow* _app_window;

public:
  CoreApp();
  HINSTANCE GetInstance() { return _hinstance; }
  virtual bool InitInstance(HINSTANCE hinstance, LPWSTR cmd_line, HWND hwnd = NULL, int screen_width = SCREEN_WIDTH, int screen_height = SCREEN_HEIGHT);

  static LRESULT CALLBACK MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  virtual void OnClose();
};

extern CoreApp* the_app_pointer;