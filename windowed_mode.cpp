#pragma once

#include <Windows.h>

#pragma comment (lib, "User32.lib")

HWND taHWND;

int TopBorder;
int BottomBorder;
int LeftBorder;
int RightBorder;

bool ApplyPatchWindowedMode();

bool setptrExe(DWORD targetp, DWORD data)
{
	DWORD protect;

	if (VirtualProtect((LPVOID)targetp, 4, PAGE_EXECUTE_READWRITE, &protect))
	{
		*(DWORD*)targetp = data;

		if (!VirtualProtect((LPVOID)targetp, 4, protect, &protect))
			return false;
	}
	else
	{
		return false;
	}

	return true;
}

bool setbyteExe(DWORD targetp, BYTE data)
{
	DWORD protect;

	if (VirtualProtect((LPVOID)targetp, 1, PAGE_EXECUTE_READWRITE, &protect))
	{
		*(BYTE*)targetp = data;

		if (!VirtualProtect((LPVOID)targetp, 1, protect, &protect))
			return false;
	}
	else
	{
		return false;
	}

	return true;
}

void GetWindowBorderSizes()
{
	RECT WindowRect;
	RECT ClientRect;
	POINT LeftTop;
	POINT RightBottom;

	GetWindowRect(taHWND, &WindowRect);
	GetClientRect(taHWND, &ClientRect);

	LeftTop.x = ClientRect.left;
	LeftTop.y = ClientRect.top;

	ClientToScreen(taHWND, &LeftTop);

	RightBottom.x = ClientRect.right;
	RightBottom.y = ClientRect.bottom;

	ClientToScreen(taHWND, &RightBottom);

	LeftBorder = LeftTop.x - WindowRect.left;
	RightBorder = WindowRect.right - RightBottom.x;
	BottomBorder = WindowRect.bottom - RightBottom.y;
	TopBorder = LeftTop.y - WindowRect.top;
}

BOOL __stdcall __GetCursorPos(LPPOINT lpPoint)
{
	BOOL ret;
	RECT lpRect;

 	ret = GetCursorPos(lpPoint);

	GetWindowRect(taHWND, &lpRect);

	lpPoint->x -= lpRect.left + LeftBorder;
	lpPoint->y -= lpRect.top + TopBorder;

	if (GetActiveWindow() == taHWND)
	{
		RECT clip = { lpRect.left + LeftBorder, lpRect.top + TopBorder, lpRect.right - RightBorder, lpRect.bottom - BottomBorder};

		ClipCursor(&clip);
	}
	else
	{
		ClipCursor(NULL);
	}

	return ret;
}

HWND WINAPI __CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	HWND hwnd = CreateWindowExA(
		dwExStyle,
		lpClassName,
		lpWindowName,
		WS_OVERLAPPED | WS_CAPTION | WS_BORDER,
		X,
		Y,
		nWidth,
		nHeight,
		hWndParent,
		hMenu,
		hInstance,
		lpParam
		);

	taHWND = hwnd;

	return hwnd;
}




BOOL WINAPI __SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	BOOL ret;

	GetWindowBorderSizes();

	ret = SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx + LeftBorder + RightBorder, cy + TopBorder + BottomBorder, uFlags);

	return ret;
}

bool PatchWindowedMode()
{
	if (!setptrExe(0x004fc334, (DWORD)__CreateWindowExA)) return false;
	if (!setptrExe(0x004fc2f0, (DWORD)__SetWindowPos)) return false;
	if (!setptrExe(0x004fc2e4, (DWORD)__GetCursorPos)) return false;
	if (!setbyteExe(0x0051fb48, 3)) return false;

	return true;
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		return ApplyPatchWindowedMode();

	return TRUE;
}

bool ApplyPatchWindowedMode()
{
	if (!PatchWindowedMode())
		return false;

	return true;
}