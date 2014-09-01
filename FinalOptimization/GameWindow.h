#pragma once

#include <Windows.h>
#include <string>

extern LRESULT CALLBACK WindowProcess(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class GameWindow
{	
public:
	class Builder
	{
	private:
		std::string mName;
		int mWidth, mHeight;
		bool mIsWindowed;
		HINSTANCE mHInstance;
		HICON mIcon;
		HCURSOR mCursor;

	public:
		Builder(HINSTANCE hInstance, std::string name = "no-name", int width=800, int height=600, bool isWindowed=true, HICON icon=nullptr, HCURSOR cursor=nullptr)
			: mHInstance(hInstance), mName(name), mWidth(width), mHeight(height), mIsWindowed(isWindowed), mIcon(icon), mCursor(cursor)
		{
		}

		const Builder& setName(const std::string& name) { mName = name; return *this; }
		const Builder& setWidth(const int width) { mWidth= width; return *this; }
		const Builder& setHeight(const int height) { mHeight = height; return *this; }
		const Builder& setWindowed(const bool isWindowed) { mIsWindowed = isWindowed; return *this; }
		const Builder& setHInstance(const HINSTANCE instance) { mHInstance = instance; return *this; }
		const Builder& setIcon(const HICON icon) { mIcon = icon; return *this; }
		const Builder& setCursor(const HCURSOR cursor) { mCursor = cursor; return *this; }

		const std::string& getName() const { return mName; }
		const HINSTANCE getHInstance() const { return mHInstance; }
		const int getWidth() const { return mWidth; }
		const int getHeight() const { return mHeight; }
		const bool getIsWindowed() const { return mIsWindowed; }
		const HICON getIcon() const { return mIcon; }
		const HCURSOR getCursor() const { return mCursor; }

		GameWindow build() { return GameWindow(*this); }
	};

private:
	bool mIsWindowed;

	HWND mHandle;
	HINSTANCE mHInstance;
	HICON mIcon;
	HCURSOR mCursor;

	// static const
	static const LPCSTR WINDOW_CLASS_NAME;

private:

	/**
	Creates a fully initialized window, but does not show it yet. To show it, invoke show();
	*/
	GameWindow(const GameWindow::Builder& builder)
		: mIsWindowed(builder.getIsWindowed()),
		mHInstance(builder.getHInstance()), mIcon(builder.getIcon()), mCursor(builder.getCursor())
	{
		// Create WNDCLASSEX
		WNDCLASSEX wcex;
		wcex.cbClsExtra = 0;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.cbWndExtra = 0;
		wcex.hbrBackground = (HBRUSH) GRAY_BRUSH;
		wcex.hCursor = mCursor;
		wcex.hIcon = mIcon;
		wcex.hIconSm = 0;
		wcex.hInstance = mHInstance;
		wcex.lpfnWndProc = WindowProcess;
		wcex.lpszClassName = WINDOW_CLASS_NAME;
		wcex.lpszMenuName = nullptr;
		wcex.style = CS_HREDRAW | CS_VREDRAW;

		RegisterClassEx(&wcex);

		mHandle = CreateWindowEx(0, WINDOW_CLASS_NAME,
			builder.getName().c_str(),
			WS_OVERLAPPEDWINDOW,
			100, 100,
			builder.getWidth(), builder.getHeight(),
			NULL, NULL, mHInstance, NULL);

		if(mHandle == 0)
			throw std::runtime_error("Window handle is 0!");
	}

public:
	/**
	Show this special window in the specified mode.
	**/
	void show(const DWORD showMode = SW_SHOW)
	{
		ShowWindow(mHandle, showMode);
	}

	~GameWindow() {
		if(mHandle)
		{
			UnregisterClass(WINDOW_CLASS_NAME, mHInstance);
		}
	}

	void setIsWindowed(const bool isWindowed) { mIsWindowed = isWindowed; }
	void setFocus() { SetFocus(mHandle); }
	void setHInstance(const HINSTANCE instance) { mHInstance = instance; }
	void setIcon(const HICON icon) { mIcon = icon; }
	void setCursor(const HCURSOR cursor) { mCursor = cursor; }
	void setTitle(const std::string& title) { SetWindowText(mHandle, title.c_str()); }


	const std::string getTitle() const { TCHAR buffer[256] = {0}; GetWindowText(mHandle, buffer, 256); return std::string(buffer) ; }
	const HINSTANCE getHInstance() const { return mHInstance; }
	int getWidth() const { RECT rect = getSize(); return (rect.right - rect.left); }
	int getHeight() const { RECT rect = getSize(); return (rect.bottom - rect.top);}
	RECT getSize() const { RECT rect; GetWindowRect(mHandle, &rect); return rect; } 
	// Returns window width/height.
	float getResolutionRatio() const { RECT rect = getSize(); return ((float)rect.right-rect.left)/((float)rect.bottom-rect.top); }
	bool getIsWindowed() const { return mIsWindowed; }
	bool getIsFocused() const { return mHandle == GetFocus(); }
	const HICON getIcon() const { return mIcon; }
	const HCURSOR getCursor() const { return mCursor; }
	const HWND getHandle() const { return mHandle; }


};
