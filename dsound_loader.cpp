#include <Windows.h>

FARPROC dsoundcreate;

__declspec(naked) HRESULT DirectSoundCreate(DWORD arg1, DWORD arg2, DWORD arg3)
{
	__asm jmp dsoundcreate;
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		TCHAR syspath[MAX_PATH];
		UINT length = GetSystemDirectory(syspath, MAX_PATH);
		lstrcpy(syspath + length, "\\dsound.dll");
		HMODULE dsound = LoadLibrary(syspath);
		if (dsound)
		{
			dsoundcreate = GetProcAddress(dsound, "DirectSoundCreate");
			if (!dsoundcreate)
				return FALSE;
		}
		else
			return FALSE;

		LoadLibrary("windowed_mode.dll");
	}

	return TRUE;
}