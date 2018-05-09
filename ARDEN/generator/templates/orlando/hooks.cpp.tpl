// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <wingdi.h>
#include <GL/gl.h>
#include "orlando/deps/MinHook.h"
#include "touchstone/platform/windows.h"

bool install_manual_hooks();

<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
extern "C" <%=func.result%> __stdcall jaq<%=func.brief_name%>(<%=table.concat(func.parameters.full, ", ")%>);
<? end ?>

static bool install_hooks()
{
	if (MH_Initialize() != MH_OK)
	{
		std::fprintf(stderr, "Couldn't initial hook library.\n");
		return false;
	}

<? for i = 1, #funcs do ?>
<? local func = funcs[i] ?>
	static LPVOID _<%=func.brief_name%>;
	if (MH_CreateHook(&<%=func.name%>, &jaq<%=func.brief_name%>, &_<%=func.brief_name%>) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "<%=func.name%>");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("<%=func.name%>", _<%=func.brief_name%>);
	}

	if (MH_EnableHook(&<%=func.name%>) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "<%=func.name%>");
		return false;
	}

<? end ?>
	if (!install_manual_hooks())
	{
		std::fprintf(stderr, "Couldn't install manual hooks.\n");
		return false;
	}

	return true;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,  DWORD fdwReason,  LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			if (!install_hooks())
			{
				std::abort();
				return FALSE;
			}
			break;
	}

	return TRUE;
}
