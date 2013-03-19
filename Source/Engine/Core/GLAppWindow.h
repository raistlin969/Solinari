#pragma once
#include <Windows.h>

class CoreApp;

class GLAppWindow
{
private:
  int _height;
  int _width;

protected:
  HGLRC _hrc;
  HDC _hdc;
  HWND _hwnd;
  HINSTANCE _hinstance;

public:
  GLAppWindow();
  ~GLAppWindow();
  bool Create(LPCWSTR title, int width, int height);
  void Resize(int w, int h);
  void Render();
};

