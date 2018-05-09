// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <windows.h>
#include "orlando/deps/MinHook.h"
#include "touchstone/platform/windows.h"

extern "C" PROC jaqGetProcAddress(LPCSTR lpszProc);
extern "C" void jaqSwapBuffers();

static BOOL (WINAPI * SwapBuffersProc)(HDC);
BOOL WINAPI orlandoSwapBuffers(HDC hdc)
{
	jaqSwapBuffers();
	return SwapBuffersProc(hdc);
}

static HHOOK WINAPI orlandoSetWindowsHookEx(int idHook, HOOKPROC lpfn, HINSTANCE hMod, DWORD dwThreadId)
{
	static int foo = 0;
	return (HHOOK)&foo;
}

bool install_manual_hooks()
{
	static LPVOID _GetProcAddress;
	if (MH_CreateHook(&wglGetProcAddress, &jaqGetProcAddress, &_GetProcAddress) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglGetProcAddress");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglGetProcAddress", _GetProcAddress);
	}

	if (MH_EnableHook(&wglGetProcAddress) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglGetProcAddress");
		return false;
	}

	static LPVOID _SetWindowsHookEx;
	if (MH_CreateHook(&SetWindowsHookEx, &orlandoSetWindowsHookEx, &_SetWindowsHookEx) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "SetWindowsHookEx");
		return false;
	}

	if (MH_EnableHook(&SetWindowsHookEx) != MH_OK)
	{
		std::fprintf(stderr, "Couldn enable '%s'.\n", "SetWindowsHookEx");
		return false;
	}

	if (MH_CreateHook(&SwapBuffers, &orlandoSwapBuffers, (LPVOID*)&SwapBuffersProc) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "SwapBuffers");
		return false;
	}

	if (MH_EnableHook(&SwapBuffers) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "SwapBuffers");
		return false;
	}

	AllocConsole();

	return true;
}
