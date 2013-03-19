#include "EngineStd.h"
#include "CoreApp.h"
#include "GLAppWindow.h"

CoreApp* the_app_pointer = 0;

CoreApp::CoreApp()
{
  the_app_pointer = this;
  _running = false;
  _quit_requested = false;
  _quitting = false;
  _app_window = 0;
}

bool CoreApp::InitInstance(HINSTANCE hinstance, LPWSTR cmd_line, HWND hwnd, int screen_width, int screen_height)
{
  _hinstance = hinstance;

  //create GLAppWindow
  _app_window = new GLAppWindow();
  _app_window->Create(L"EngineTest", 800, 600);
  _running = true;
  return true;
}

void CoreApp::OnClose()
{
  _quit_requested = true;
  _quitting = true;
  delete _app_window;
}

LRESULT CALLBACK CoreApp::MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
  LRESULT result = 0;

  switch(msg)
  {
  case WM_CLOSE:
    the_app_pointer->OnClose();
    break;

  default:
    result = DefWindowProc(hwnd, msg, wparam, lparam);
  }
  return result;
}