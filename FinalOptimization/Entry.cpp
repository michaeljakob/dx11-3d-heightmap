#include "Entry.h"
#include "FinalOptimization.h"


int WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int cmdShow)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	HICON icon = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));


	GameWindow::Builder winBuilder(instance, "3d terrain | (c) Michael Jakob, michael@jakob.tv", static_cast<int>(desktop.right * 0.7), static_cast<int>(desktop.bottom * 0.7), true, icon);
	FinalOptimization game(winBuilder);

	return 0;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
LRESULT CALLBACK WindowProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	//case WM_CHAR:
	//	std::c out << "Key pressed: " << (char)wParam << std::endl;
	//	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}