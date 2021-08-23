/// @file
/// This file contains the interface of a simpel custom win32 window. The interface is very simple
/// as the window (control) is just a simple demonstration. See the corresponding cpp file.
/// -Adel Amro (http://code-section.com)


# include <Windows.h>



namespace CustomWindow
{

/// The name used to register the window class with the OS. It can be used to test whether
/// a window is of a specific class or not.
extern LPCTSTR WC_CUSTOMWINDOW;

/// Sets the color of the specified custom window (a window created with the WC_CUSTOMWINDOW class).
/// If a window of a different class is passed, an error will occur.
VOID SetColor( HWND hWnd, COLORREF color, BOOL bRepaint = TRUE );

/// Initialize the custom window class. This must be called before creating any windows of class WC_CUSTOMWINDOW.
BOOL Initialize( HINSTANCE hInstance );

/// Should be called at the end of the application to unregister the custom window class.
VOID Uninitialize( HINSTANCE hInstance );


};