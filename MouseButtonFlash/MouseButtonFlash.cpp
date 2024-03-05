#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

#include <cstdio>
#include <iostream>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((unsigned short) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((unsigned short) 0x02)
#endif


int mouse_button = 0;


LRESULT CALLBACK EventHandler(HWND hwnd, unsigned event, WPARAM wparam, LPARAM lparam)
{
	switch (event)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_INPUT:
	{
		unsigned size = sizeof(RAWINPUT);
		static RAWINPUT raw;
		GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

		if (raw.header.dwType == RIM_TYPEMOUSE) {
			mouse_button = raw.data.mouse.usButtonFlags;
		}
	}
	return 0;
	}

	return DefWindowProc(hwnd, event, wparam, lparam);
}


int main(int argc, char **argv)
{
	HINSTANCE instance = GetModuleHandle(0);

	int left = 10;
	int top = 10;
	int right = 90;
	int bottom = 90;

	if (argc > 4)
	{
		left = atoi(argv[1]);
		top = atoi(argv[2]);
		right = atoi(argv[3]);
		bottom = atoi(argv[4]);
	}

	if (!AllocConsole())
	{
		return -1;
	}

	LPCWSTR class_name = L"Mouse Button Tester";

	WNDCLASS window_class = {};
	window_class.lpfnWndProc = EventHandler;
	window_class.hInstance = instance;
	window_class.lpszClassName = class_name;

	if (!RegisterClass(&window_class))
		return -1;

	HWND window = CreateWindow(class_name, class_name, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);

	if (window == nullptr)
		return -1;


	RAWINPUTDEVICE rid[1];
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = window;
	RegisterRawInputDevices(rid, 1, sizeof(rid[0]));

	MSG event;
	bool quit = false;
	bool buttonWasDown = false;

	HDC hdc = GetDC(NULL);
	RECT rect;
	SetRect(&rect, left, top, right, bottom);
	HBRUSH hRed = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH hBlue = CreateSolidBrush(RGB(0, 0, 255));

	while (!quit)
	{
		while (PeekMessage(&event, 0, 0, 0, PM_REMOVE))
		{
			if (event.message == WM_QUIT)
			{
				quit = true;
				break;
			}

			TranslateMessage(&event);
			DispatchMessage(&event);
		}

		if (mouse_button & 0x01)
		{
			if (!buttonWasDown)
			{
				buttonWasDown = true;
				FillRect(hdc, &rect, hRed);
			}
		}
		else if (mouse_button & 0x02)
		{
			if (buttonWasDown)
			{
				buttonWasDown = false;
				FillRect(hdc, &rect, hBlue);
			}
		}

		mouse_button = 0;
	}

	ReleaseDC(NULL, hdc);

	return 0;
}

