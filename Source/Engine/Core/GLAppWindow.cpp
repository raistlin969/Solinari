#include "EngineStd.h"
#include "GLAppWindow.h"
#include "CoreApp.h"

#include <gl/glew.h>
#include <gl/wglew.h>

GLAppWindow::GLAppWindow()
{
}

GLAppWindow::~GLAppWindow()
{
  wglMakeCurrent(_hdc, 0);  //remove rendering context from device context
  wglDeleteContext(_hrc);   //delete rendering context
  ReleaseDC(_hwnd, _hdc);   //release device context from window
  PostMessage(_hwnd, WM_QUIT, 0, 0);
  DestroyWindow(_hwnd);
}

bool GLAppWindow::Create(LPCWSTR title, int width, int height)
{
  WNDCLASS window_class;
  DWORD ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
  _hinstance = GetModuleHandle(NULL);
  window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  window_class.lpfnWndProc = (WNDPROC) CoreApp::MsgProc;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = _hinstance;
  window_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  window_class.hbrBackground = NULL;
  window_class.lpszMenuName = NULL;
  window_class.lpszClassName = title;

  if(!RegisterClass(&window_class))
    return false;

  _hwnd = CreateWindowEx(ex_style, title, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, _width, _height, NULL, NULL, _hinstance, NULL);

  //create opengl context
  _hdc = GetDC(_hwnd);
  PIXELFORMATDESCRIPTOR pfd; // Create a new PIXELFORMATDESCRIPTOR (PFD)
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window
	pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels
	pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)
	pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)
	pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD

	int nPixelFormat = ChoosePixelFormat(_hdc, &pfd); // Check if our PFD is valid and get a pixel format back
	if (nPixelFormat == 0) // If it fails
			return false;

	BOOL bResult = SetPixelFormat(_hdc, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD
	if (!bResult) // If it fails
		return false;

	HGLRC tempOpenGLContext = wglCreateContext(_hdc); // Create an OpenGL 2.1 context for our device context
	wglMakeCurrent(_hdc, tempOpenGLContext); // Make the OpenGL 2.1 context current and active

	GLenum error = glewInit(); // Enable GLEW
	if (error != GLEW_OK) // If GLEW fails
		return false;

	int attributes[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3, // Set the MAJOR version of OpenGL to 3
		WGL_CONTEXT_MINOR_VERSION_ARB, 2, // Set the MINOR version of OpenGL to 2
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // Set our OpenGL context to be forward compatible
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1) { // If the OpenGL 3.x context creation extension is available
		_hrc = wglCreateContextAttribsARB(_hdc, NULL, attributes); // Create and OpenGL 3.x context based on the given attributes
		wglMakeCurrent(NULL, NULL); // Remove the temporary context from being active
		wglDeleteContext(tempOpenGLContext); // Delete the temporary OpenGL 2.1 context
		wglMakeCurrent(_hdc, _hrc); // Make our OpenGL 3.0 context current
	}
	else {
		_hrc = tempOpenGLContext; // If we didn't have support for OpenGL 3.x and up, use the OpenGL 2.1 context
	}

	int glVersion[2] = {-1, -1}; // Set some default values for the version
	glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]); // Get back the OpenGL MAJOR version we are using
	glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]); // Get back the OpenGL MAJOR version we are using


  ShowWindow(_hwnd, SW_SHOW);
  UpdateWindow(_hwnd);
  return true;
}

void GLAppWindow::Resize(int w, int h)
{
}

void GLAppWindow::Render()
{
}
