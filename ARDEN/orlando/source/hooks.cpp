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

extern "C" void __stdcall jaqBindTexture(GLenum target, GLuint texture);
extern "C" void __stdcall jaqBlendFunc(GLenum sfactor, GLenum dfactor);
extern "C" void __stdcall jaqClear(GLbitfield mask);
extern "C" void __stdcall jaqClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern "C" void __stdcall jaqClearDepth(GLdouble depth);
extern "C" void __stdcall jaqClearStencil(GLint s);
extern "C" void __stdcall jaqColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
extern "C" void __stdcall jaqCopyTexImage1D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
extern "C" void __stdcall jaqCopyTexImage2D(GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
extern "C" void __stdcall jaqCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
extern "C" void __stdcall jaqCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
extern "C" void __stdcall jaqCullFace(GLenum mode);
extern "C" void __stdcall jaqDeleteTextures(GLsizei n, const GLuint * textures);
extern "C" void __stdcall jaqDepthFunc(GLenum func);
extern "C" void __stdcall jaqDepthMask(GLboolean flag);
extern "C" void __stdcall jaqDepthRange(GLdouble near, GLdouble far);
extern "C" void __stdcall jaqDisable(GLenum cap);
extern "C" void __stdcall jaqDrawArrays(GLenum mode, GLint first, GLsizei count);
extern "C" void __stdcall jaqDrawBuffer(GLenum buf);
extern "C" void __stdcall jaqDrawElements(GLenum mode, GLsizei count, GLenum type, const void * indices);
extern "C" void __stdcall jaqEnable(GLenum cap);
extern "C" void __stdcall jaqFinish();
extern "C" void __stdcall jaqFlush();
extern "C" void __stdcall jaqFrontFace(GLenum mode);
extern "C" void __stdcall jaqGenTextures(GLsizei n, GLuint * textures);
extern "C" void __stdcall jaqGetBooleanv(GLenum pname, GLboolean * data);
extern "C" void __stdcall jaqGetDoublev(GLenum pname, GLdouble * data);
extern "C" GLenum __stdcall jaqGetError();
extern "C" void __stdcall jaqGetFloatv(GLenum pname, GLfloat * data);
extern "C" void __stdcall jaqGetIntegerv(GLenum pname, GLint * data);
extern "C" const GLubyte * __stdcall jaqGetString(GLenum name);
extern "C" void __stdcall jaqGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void * pixels);
extern "C" void __stdcall jaqGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat * params);
extern "C" void __stdcall jaqGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint * params);
extern "C" void __stdcall jaqGetTexParameterfv(GLenum target, GLenum pname, GLfloat * params);
extern "C" void __stdcall jaqGetTexParameteriv(GLenum target, GLenum pname, GLint * params);
extern "C" void __stdcall jaqHint(GLenum target, GLenum mode);
extern "C" GLboolean __stdcall jaqIsEnabled(GLenum cap);
extern "C" GLboolean __stdcall jaqIsTexture(GLuint texture);
extern "C" void __stdcall jaqLineWidth(GLfloat width);
extern "C" void __stdcall jaqLogicOp(GLenum opcode);
extern "C" void __stdcall jaqPixelStoref(GLenum pname, GLfloat param);
extern "C" void __stdcall jaqPixelStorei(GLenum pname, GLint param);
extern "C" void __stdcall jaqPointSize(GLfloat size);
extern "C" void __stdcall jaqPolygonMode(GLenum face, GLenum mode);
extern "C" void __stdcall jaqPolygonOffset(GLfloat factor, GLfloat units);
extern "C" void __stdcall jaqReadBuffer(GLenum src);
extern "C" void __stdcall jaqReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void * pixels);
extern "C" void __stdcall jaqScissor(GLint x, GLint y, GLsizei width, GLsizei height);
extern "C" void __stdcall jaqStencilFunc(GLenum func, GLint ref, GLuint mask);
extern "C" void __stdcall jaqStencilMask(GLuint mask);
extern "C" void __stdcall jaqStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
extern "C" void __stdcall jaqTexImage1D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const void * pixels);
extern "C" void __stdcall jaqTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void * pixels);
extern "C" void __stdcall jaqTexParameterf(GLenum target, GLenum pname, GLfloat param);
extern "C" void __stdcall jaqTexParameterfv(GLenum target, GLenum pname, const GLfloat * params);
extern "C" void __stdcall jaqTexParameteri(GLenum target, GLenum pname, GLint param);
extern "C" void __stdcall jaqTexParameteriv(GLenum target, GLenum pname, const GLint * params);
extern "C" void __stdcall jaqTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void * pixels);
extern "C" void __stdcall jaqTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels);
extern "C" void __stdcall jaqViewport(GLint x, GLint y, GLsizei width, GLsizei height);
extern "C" BOOL __stdcall jaqCopyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask);
extern "C" HGLRC __stdcall jaqCreateContext(HDC hdc);
extern "C" HGLRC __stdcall jaqCreateLayerContext(HDC hdc, int iLayerPlane);
extern "C" BOOL __stdcall jaqDeleteContext(HGLRC hglrc);
extern "C" BOOL __stdcall jaqDescribeLayerPlane(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd);
extern "C" HGLRC __stdcall jaqGetCurrentContext();
extern "C" HDC __stdcall jaqGetCurrentDC();
extern "C" int __stdcall jaqGetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF* pcr);
extern "C" BOOL __stdcall jaqMakeCurrent(HDC hdc, HGLRC hglrc);
extern "C" BOOL __stdcall jaqRealizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize);
extern "C" int __stdcall jaqSetLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart, int cEntries, const COLORREF* pcr);
extern "C" BOOL __stdcall jaqShareLists(HGLRC hglrc1, HGLRC hglrc2);
extern "C" BOOL __stdcall jaqSwapLayerBuffers(HDC hdc, UINT fuPlanes);
extern "C" BOOL __stdcall jaqUseFontBitmapsA(HDC hdc, DWORD first, DWORD count, DWORD base);
extern "C" BOOL __stdcall jaqUseFontBitmapsW(HDC hdc, DWORD first, DWORD count, DWORD base);
extern "C" BOOL __stdcall jaqUseFontOutlines(HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf);

static bool install_hooks()
{
	if (MH_Initialize() != MH_OK)
	{
		std::fprintf(stderr, "Couldn't initial hook library.\n");
		return false;
	}

	static LPVOID _BindTexture;
	if (MH_CreateHook(&glBindTexture, &jaqBindTexture, &_BindTexture) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glBindTexture");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glBindTexture", _BindTexture);
	}

	if (MH_EnableHook(&glBindTexture) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glBindTexture");
		return false;
	}

	static LPVOID _BlendFunc;
	if (MH_CreateHook(&glBlendFunc, &jaqBlendFunc, &_BlendFunc) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glBlendFunc");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glBlendFunc", _BlendFunc);
	}

	if (MH_EnableHook(&glBlendFunc) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glBlendFunc");
		return false;
	}

	static LPVOID _Clear;
	if (MH_CreateHook(&glClear, &jaqClear, &_Clear) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glClear");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glClear", _Clear);
	}

	if (MH_EnableHook(&glClear) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glClear");
		return false;
	}

	static LPVOID _ClearColor;
	if (MH_CreateHook(&glClearColor, &jaqClearColor, &_ClearColor) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glClearColor");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glClearColor", _ClearColor);
	}

	if (MH_EnableHook(&glClearColor) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glClearColor");
		return false;
	}

	static LPVOID _ClearDepth;
	if (MH_CreateHook(&glClearDepth, &jaqClearDepth, &_ClearDepth) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glClearDepth");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glClearDepth", _ClearDepth);
	}

	if (MH_EnableHook(&glClearDepth) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glClearDepth");
		return false;
	}

	static LPVOID _ClearStencil;
	if (MH_CreateHook(&glClearStencil, &jaqClearStencil, &_ClearStencil) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glClearStencil");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glClearStencil", _ClearStencil);
	}

	if (MH_EnableHook(&glClearStencil) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glClearStencil");
		return false;
	}

	static LPVOID _ColorMask;
	if (MH_CreateHook(&glColorMask, &jaqColorMask, &_ColorMask) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glColorMask");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glColorMask", _ColorMask);
	}

	if (MH_EnableHook(&glColorMask) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glColorMask");
		return false;
	}

	static LPVOID _CopyTexImage1D;
	if (MH_CreateHook(&glCopyTexImage1D, &jaqCopyTexImage1D, &_CopyTexImage1D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glCopyTexImage1D");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glCopyTexImage1D", _CopyTexImage1D);
	}

	if (MH_EnableHook(&glCopyTexImage1D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glCopyTexImage1D");
		return false;
	}

	static LPVOID _CopyTexImage2D;
	if (MH_CreateHook(&glCopyTexImage2D, &jaqCopyTexImage2D, &_CopyTexImage2D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glCopyTexImage2D");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glCopyTexImage2D", _CopyTexImage2D);
	}

	if (MH_EnableHook(&glCopyTexImage2D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glCopyTexImage2D");
		return false;
	}

	static LPVOID _CopyTexSubImage1D;
	if (MH_CreateHook(&glCopyTexSubImage1D, &jaqCopyTexSubImage1D, &_CopyTexSubImage1D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glCopyTexSubImage1D");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glCopyTexSubImage1D", _CopyTexSubImage1D);
	}

	if (MH_EnableHook(&glCopyTexSubImage1D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glCopyTexSubImage1D");
		return false;
	}

	static LPVOID _CopyTexSubImage2D;
	if (MH_CreateHook(&glCopyTexSubImage2D, &jaqCopyTexSubImage2D, &_CopyTexSubImage2D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glCopyTexSubImage2D");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glCopyTexSubImage2D", _CopyTexSubImage2D);
	}

	if (MH_EnableHook(&glCopyTexSubImage2D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glCopyTexSubImage2D");
		return false;
	}

	static LPVOID _CullFace;
	if (MH_CreateHook(&glCullFace, &jaqCullFace, &_CullFace) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glCullFace");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glCullFace", _CullFace);
	}

	if (MH_EnableHook(&glCullFace) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glCullFace");
		return false;
	}

	static LPVOID _DeleteTextures;
	if (MH_CreateHook(&glDeleteTextures, &jaqDeleteTextures, &_DeleteTextures) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glDeleteTextures");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glDeleteTextures", _DeleteTextures);
	}

	if (MH_EnableHook(&glDeleteTextures) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glDeleteTextures");
		return false;
	}

	static LPVOID _DepthFunc;
	if (MH_CreateHook(&glDepthFunc, &jaqDepthFunc, &_DepthFunc) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glDepthFunc");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glDepthFunc", _DepthFunc);
	}

	if (MH_EnableHook(&glDepthFunc) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glDepthFunc");
		return false;
	}

	static LPVOID _DepthMask;
	if (MH_CreateHook(&glDepthMask, &jaqDepthMask, &_DepthMask) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glDepthMask");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glDepthMask", _DepthMask);
	}

	if (MH_EnableHook(&glDepthMask) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glDepthMask");
		return false;
	}

	static LPVOID _DepthRange;
	if (MH_CreateHook(&glDepthRange, &jaqDepthRange, &_DepthRange) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glDepthRange");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glDepthRange", _DepthRange);
	}

	if (MH_EnableHook(&glDepthRange) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glDepthRange");
		return false;
	}

	static LPVOID _Disable;
	if (MH_CreateHook(&glDisable, &jaqDisable, &_Disable) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glDisable");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glDisable", _Disable);
	}

	if (MH_EnableHook(&glDisable) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glDisable");
		return false;
	}

	static LPVOID _DrawArrays;
	if (MH_CreateHook(&glDrawArrays, &jaqDrawArrays, &_DrawArrays) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glDrawArrays");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glDrawArrays", _DrawArrays);
	}

	if (MH_EnableHook(&glDrawArrays) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glDrawArrays");
		return false;
	}

	static LPVOID _DrawBuffer;
	if (MH_CreateHook(&glDrawBuffer, &jaqDrawBuffer, &_DrawBuffer) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glDrawBuffer");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glDrawBuffer", _DrawBuffer);
	}

	if (MH_EnableHook(&glDrawBuffer) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glDrawBuffer");
		return false;
	}

	static LPVOID _DrawElements;
	if (MH_CreateHook(&glDrawElements, &jaqDrawElements, &_DrawElements) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glDrawElements");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glDrawElements", _DrawElements);
	}

	if (MH_EnableHook(&glDrawElements) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glDrawElements");
		return false;
	}

	static LPVOID _Enable;
	if (MH_CreateHook(&glEnable, &jaqEnable, &_Enable) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glEnable");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glEnable", _Enable);
	}

	if (MH_EnableHook(&glEnable) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glEnable");
		return false;
	}

	static LPVOID _Finish;
	if (MH_CreateHook(&glFinish, &jaqFinish, &_Finish) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glFinish");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glFinish", _Finish);
	}

	if (MH_EnableHook(&glFinish) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glFinish");
		return false;
	}

	static LPVOID _Flush;
	if (MH_CreateHook(&glFlush, &jaqFlush, &_Flush) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glFlush");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glFlush", _Flush);
	}

	if (MH_EnableHook(&glFlush) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glFlush");
		return false;
	}

	static LPVOID _FrontFace;
	if (MH_CreateHook(&glFrontFace, &jaqFrontFace, &_FrontFace) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glFrontFace");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glFrontFace", _FrontFace);
	}

	if (MH_EnableHook(&glFrontFace) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glFrontFace");
		return false;
	}

	static LPVOID _GenTextures;
	if (MH_CreateHook(&glGenTextures, &jaqGenTextures, &_GenTextures) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGenTextures");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGenTextures", _GenTextures);
	}

	if (MH_EnableHook(&glGenTextures) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGenTextures");
		return false;
	}

	static LPVOID _GetBooleanv;
	if (MH_CreateHook(&glGetBooleanv, &jaqGetBooleanv, &_GetBooleanv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetBooleanv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetBooleanv", _GetBooleanv);
	}

	if (MH_EnableHook(&glGetBooleanv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetBooleanv");
		return false;
	}

	static LPVOID _GetDoublev;
	if (MH_CreateHook(&glGetDoublev, &jaqGetDoublev, &_GetDoublev) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetDoublev");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetDoublev", _GetDoublev);
	}

	if (MH_EnableHook(&glGetDoublev) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetDoublev");
		return false;
	}

	static LPVOID _GetError;
	if (MH_CreateHook(&glGetError, &jaqGetError, &_GetError) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetError");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetError", _GetError);
	}

	if (MH_EnableHook(&glGetError) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetError");
		return false;
	}

	static LPVOID _GetFloatv;
	if (MH_CreateHook(&glGetFloatv, &jaqGetFloatv, &_GetFloatv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetFloatv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetFloatv", _GetFloatv);
	}

	if (MH_EnableHook(&glGetFloatv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetFloatv");
		return false;
	}

	static LPVOID _GetIntegerv;
	if (MH_CreateHook(&glGetIntegerv, &jaqGetIntegerv, &_GetIntegerv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetIntegerv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetIntegerv", _GetIntegerv);
	}

	if (MH_EnableHook(&glGetIntegerv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetIntegerv");
		return false;
	}

	static LPVOID _GetString;
	if (MH_CreateHook(&glGetString, &jaqGetString, &_GetString) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetString");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetString", _GetString);
	}

	if (MH_EnableHook(&glGetString) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetString");
		return false;
	}

	static LPVOID _GetTexImage;
	if (MH_CreateHook(&glGetTexImage, &jaqGetTexImage, &_GetTexImage) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetTexImage");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetTexImage", _GetTexImage);
	}

	if (MH_EnableHook(&glGetTexImage) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetTexImage");
		return false;
	}

	static LPVOID _GetTexLevelParameterfv;
	if (MH_CreateHook(&glGetTexLevelParameterfv, &jaqGetTexLevelParameterfv, &_GetTexLevelParameterfv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetTexLevelParameterfv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetTexLevelParameterfv", _GetTexLevelParameterfv);
	}

	if (MH_EnableHook(&glGetTexLevelParameterfv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetTexLevelParameterfv");
		return false;
	}

	static LPVOID _GetTexLevelParameteriv;
	if (MH_CreateHook(&glGetTexLevelParameteriv, &jaqGetTexLevelParameteriv, &_GetTexLevelParameteriv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetTexLevelParameteriv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetTexLevelParameteriv", _GetTexLevelParameteriv);
	}

	if (MH_EnableHook(&glGetTexLevelParameteriv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetTexLevelParameteriv");
		return false;
	}

	static LPVOID _GetTexParameterfv;
	if (MH_CreateHook(&glGetTexParameterfv, &jaqGetTexParameterfv, &_GetTexParameterfv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetTexParameterfv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetTexParameterfv", _GetTexParameterfv);
	}

	if (MH_EnableHook(&glGetTexParameterfv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetTexParameterfv");
		return false;
	}

	static LPVOID _GetTexParameteriv;
	if (MH_CreateHook(&glGetTexParameteriv, &jaqGetTexParameteriv, &_GetTexParameteriv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glGetTexParameteriv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glGetTexParameteriv", _GetTexParameteriv);
	}

	if (MH_EnableHook(&glGetTexParameteriv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glGetTexParameteriv");
		return false;
	}

	static LPVOID _Hint;
	if (MH_CreateHook(&glHint, &jaqHint, &_Hint) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glHint");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glHint", _Hint);
	}

	if (MH_EnableHook(&glHint) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glHint");
		return false;
	}

	static LPVOID _IsEnabled;
	if (MH_CreateHook(&glIsEnabled, &jaqIsEnabled, &_IsEnabled) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glIsEnabled");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glIsEnabled", _IsEnabled);
	}

	if (MH_EnableHook(&glIsEnabled) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glIsEnabled");
		return false;
	}

	static LPVOID _IsTexture;
	if (MH_CreateHook(&glIsTexture, &jaqIsTexture, &_IsTexture) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glIsTexture");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glIsTexture", _IsTexture);
	}

	if (MH_EnableHook(&glIsTexture) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glIsTexture");
		return false;
	}

	static LPVOID _LineWidth;
	if (MH_CreateHook(&glLineWidth, &jaqLineWidth, &_LineWidth) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glLineWidth");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glLineWidth", _LineWidth);
	}

	if (MH_EnableHook(&glLineWidth) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glLineWidth");
		return false;
	}

	static LPVOID _LogicOp;
	if (MH_CreateHook(&glLogicOp, &jaqLogicOp, &_LogicOp) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glLogicOp");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glLogicOp", _LogicOp);
	}

	if (MH_EnableHook(&glLogicOp) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glLogicOp");
		return false;
	}

	static LPVOID _PixelStoref;
	if (MH_CreateHook(&glPixelStoref, &jaqPixelStoref, &_PixelStoref) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glPixelStoref");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glPixelStoref", _PixelStoref);
	}

	if (MH_EnableHook(&glPixelStoref) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glPixelStoref");
		return false;
	}

	static LPVOID _PixelStorei;
	if (MH_CreateHook(&glPixelStorei, &jaqPixelStorei, &_PixelStorei) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glPixelStorei");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glPixelStorei", _PixelStorei);
	}

	if (MH_EnableHook(&glPixelStorei) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glPixelStorei");
		return false;
	}

	static LPVOID _PointSize;
	if (MH_CreateHook(&glPointSize, &jaqPointSize, &_PointSize) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glPointSize");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glPointSize", _PointSize);
	}

	if (MH_EnableHook(&glPointSize) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glPointSize");
		return false;
	}

	static LPVOID _PolygonMode;
	if (MH_CreateHook(&glPolygonMode, &jaqPolygonMode, &_PolygonMode) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glPolygonMode");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glPolygonMode", _PolygonMode);
	}

	if (MH_EnableHook(&glPolygonMode) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glPolygonMode");
		return false;
	}

	static LPVOID _PolygonOffset;
	if (MH_CreateHook(&glPolygonOffset, &jaqPolygonOffset, &_PolygonOffset) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glPolygonOffset");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glPolygonOffset", _PolygonOffset);
	}

	if (MH_EnableHook(&glPolygonOffset) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glPolygonOffset");
		return false;
	}

	static LPVOID _ReadBuffer;
	if (MH_CreateHook(&glReadBuffer, &jaqReadBuffer, &_ReadBuffer) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glReadBuffer");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glReadBuffer", _ReadBuffer);
	}

	if (MH_EnableHook(&glReadBuffer) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glReadBuffer");
		return false;
	}

	static LPVOID _ReadPixels;
	if (MH_CreateHook(&glReadPixels, &jaqReadPixels, &_ReadPixels) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glReadPixels");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glReadPixels", _ReadPixels);
	}

	if (MH_EnableHook(&glReadPixels) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glReadPixels");
		return false;
	}

	static LPVOID _Scissor;
	if (MH_CreateHook(&glScissor, &jaqScissor, &_Scissor) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glScissor");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glScissor", _Scissor);
	}

	if (MH_EnableHook(&glScissor) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glScissor");
		return false;
	}

	static LPVOID _StencilFunc;
	if (MH_CreateHook(&glStencilFunc, &jaqStencilFunc, &_StencilFunc) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glStencilFunc");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glStencilFunc", _StencilFunc);
	}

	if (MH_EnableHook(&glStencilFunc) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glStencilFunc");
		return false;
	}

	static LPVOID _StencilMask;
	if (MH_CreateHook(&glStencilMask, &jaqStencilMask, &_StencilMask) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glStencilMask");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glStencilMask", _StencilMask);
	}

	if (MH_EnableHook(&glStencilMask) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glStencilMask");
		return false;
	}

	static LPVOID _StencilOp;
	if (MH_CreateHook(&glStencilOp, &jaqStencilOp, &_StencilOp) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glStencilOp");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glStencilOp", _StencilOp);
	}

	if (MH_EnableHook(&glStencilOp) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glStencilOp");
		return false;
	}

	static LPVOID _TexImage1D;
	if (MH_CreateHook(&glTexImage1D, &jaqTexImage1D, &_TexImage1D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glTexImage1D");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glTexImage1D", _TexImage1D);
	}

	if (MH_EnableHook(&glTexImage1D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glTexImage1D");
		return false;
	}

	static LPVOID _TexImage2D;
	if (MH_CreateHook(&glTexImage2D, &jaqTexImage2D, &_TexImage2D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glTexImage2D");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glTexImage2D", _TexImage2D);
	}

	if (MH_EnableHook(&glTexImage2D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glTexImage2D");
		return false;
	}

	static LPVOID _TexParameterf;
	if (MH_CreateHook(&glTexParameterf, &jaqTexParameterf, &_TexParameterf) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glTexParameterf");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glTexParameterf", _TexParameterf);
	}

	if (MH_EnableHook(&glTexParameterf) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glTexParameterf");
		return false;
	}

	static LPVOID _TexParameterfv;
	if (MH_CreateHook(&glTexParameterfv, &jaqTexParameterfv, &_TexParameterfv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glTexParameterfv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glTexParameterfv", _TexParameterfv);
	}

	if (MH_EnableHook(&glTexParameterfv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glTexParameterfv");
		return false;
	}

	static LPVOID _TexParameteri;
	if (MH_CreateHook(&glTexParameteri, &jaqTexParameteri, &_TexParameteri) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glTexParameteri");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glTexParameteri", _TexParameteri);
	}

	if (MH_EnableHook(&glTexParameteri) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glTexParameteri");
		return false;
	}

	static LPVOID _TexParameteriv;
	if (MH_CreateHook(&glTexParameteriv, &jaqTexParameteriv, &_TexParameteriv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glTexParameteriv");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glTexParameteriv", _TexParameteriv);
	}

	if (MH_EnableHook(&glTexParameteriv) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glTexParameteriv");
		return false;
	}

	static LPVOID _TexSubImage1D;
	if (MH_CreateHook(&glTexSubImage1D, &jaqTexSubImage1D, &_TexSubImage1D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glTexSubImage1D");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glTexSubImage1D", _TexSubImage1D);
	}

	if (MH_EnableHook(&glTexSubImage1D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glTexSubImage1D");
		return false;
	}

	static LPVOID _TexSubImage2D;
	if (MH_CreateHook(&glTexSubImage2D, &jaqTexSubImage2D, &_TexSubImage2D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glTexSubImage2D");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glTexSubImage2D", _TexSubImage2D);
	}

	if (MH_EnableHook(&glTexSubImage2D) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glTexSubImage2D");
		return false;
	}

	static LPVOID _Viewport;
	if (MH_CreateHook(&glViewport, &jaqViewport, &_Viewport) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "glViewport");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("glViewport", _Viewport);
	}

	if (MH_EnableHook(&glViewport) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "glViewport");
		return false;
	}

	static LPVOID _CopyContext;
	if (MH_CreateHook(&wglCopyContext, &jaqCopyContext, &_CopyContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglCopyContext");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglCopyContext", _CopyContext);
	}

	if (MH_EnableHook(&wglCopyContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglCopyContext");
		return false;
	}

	static LPVOID _CreateContext;
	if (MH_CreateHook(&wglCreateContext, &jaqCreateContext, &_CreateContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglCreateContext");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglCreateContext", _CreateContext);
	}

	if (MH_EnableHook(&wglCreateContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglCreateContext");
		return false;
	}

	static LPVOID _CreateLayerContext;
	if (MH_CreateHook(&wglCreateLayerContext, &jaqCreateLayerContext, &_CreateLayerContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglCreateLayerContext");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglCreateLayerContext", _CreateLayerContext);
	}

	if (MH_EnableHook(&wglCreateLayerContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglCreateLayerContext");
		return false;
	}

	static LPVOID _DeleteContext;
	if (MH_CreateHook(&wglDeleteContext, &jaqDeleteContext, &_DeleteContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglDeleteContext");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglDeleteContext", _DeleteContext);
	}

	if (MH_EnableHook(&wglDeleteContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglDeleteContext");
		return false;
	}

	static LPVOID _DescribeLayerPlane;
	if (MH_CreateHook(&wglDescribeLayerPlane, &jaqDescribeLayerPlane, &_DescribeLayerPlane) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglDescribeLayerPlane");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglDescribeLayerPlane", _DescribeLayerPlane);
	}

	if (MH_EnableHook(&wglDescribeLayerPlane) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglDescribeLayerPlane");
		return false;
	}

	static LPVOID _GetCurrentContext;
	if (MH_CreateHook(&wglGetCurrentContext, &jaqGetCurrentContext, &_GetCurrentContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglGetCurrentContext");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglGetCurrentContext", _GetCurrentContext);
	}

	if (MH_EnableHook(&wglGetCurrentContext) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglGetCurrentContext");
		return false;
	}

	static LPVOID _GetCurrentDC;
	if (MH_CreateHook(&wglGetCurrentDC, &jaqGetCurrentDC, &_GetCurrentDC) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglGetCurrentDC");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglGetCurrentDC", _GetCurrentDC);
	}

	if (MH_EnableHook(&wglGetCurrentDC) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglGetCurrentDC");
		return false;
	}

	static LPVOID _GetLayerPaletteEntries;
	if (MH_CreateHook(&wglGetLayerPaletteEntries, &jaqGetLayerPaletteEntries, &_GetLayerPaletteEntries) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglGetLayerPaletteEntries");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglGetLayerPaletteEntries", _GetLayerPaletteEntries);
	}

	if (MH_EnableHook(&wglGetLayerPaletteEntries) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglGetLayerPaletteEntries");
		return false;
	}

	static LPVOID _MakeCurrent;
	if (MH_CreateHook(&wglMakeCurrent, &jaqMakeCurrent, &_MakeCurrent) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglMakeCurrent");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglMakeCurrent", _MakeCurrent);
	}

	if (MH_EnableHook(&wglMakeCurrent) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglMakeCurrent");
		return false;
	}

	static LPVOID _RealizeLayerPalette;
	if (MH_CreateHook(&wglRealizeLayerPalette, &jaqRealizeLayerPalette, &_RealizeLayerPalette) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglRealizeLayerPalette");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglRealizeLayerPalette", _RealizeLayerPalette);
	}

	if (MH_EnableHook(&wglRealizeLayerPalette) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglRealizeLayerPalette");
		return false;
	}

	static LPVOID _SetLayerPaletteEntries;
	if (MH_CreateHook(&wglSetLayerPaletteEntries, &jaqSetLayerPaletteEntries, &_SetLayerPaletteEntries) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglSetLayerPaletteEntries");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglSetLayerPaletteEntries", _SetLayerPaletteEntries);
	}

	if (MH_EnableHook(&wglSetLayerPaletteEntries) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglSetLayerPaletteEntries");
		return false;
	}

	static LPVOID _ShareLists;
	if (MH_CreateHook(&wglShareLists, &jaqShareLists, &_ShareLists) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglShareLists");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglShareLists", _ShareLists);
	}

	if (MH_EnableHook(&wglShareLists) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglShareLists");
		return false;
	}

	static LPVOID _SwapLayerBuffers;
	if (MH_CreateHook(&wglSwapLayerBuffers, &jaqSwapLayerBuffers, &_SwapLayerBuffers) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglSwapLayerBuffers");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglSwapLayerBuffers", _SwapLayerBuffers);
	}

	if (MH_EnableHook(&wglSwapLayerBuffers) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglSwapLayerBuffers");
		return false;
	}

	static LPVOID _UseFontBitmapsA;
	if (MH_CreateHook(&wglUseFontBitmapsA, &jaqUseFontBitmapsA, &_UseFontBitmapsA) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglUseFontBitmapsA");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglUseFontBitmapsA", _UseFontBitmapsA);
	}

	if (MH_EnableHook(&wglUseFontBitmapsA) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglUseFontBitmapsA");
		return false;
	}

	static LPVOID _UseFontBitmapsW;
	if (MH_CreateHook(&wglUseFontBitmapsW, &jaqUseFontBitmapsW, &_UseFontBitmapsW) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglUseFontBitmapsW");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglUseFontBitmapsW", _UseFontBitmapsW);
	}

	if (MH_EnableHook(&wglUseFontBitmapsW) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglUseFontBitmapsW");
		return false;
	}

	static LPVOID _UseFontOutlines;
	if (MH_CreateHook(&wglUseFontOutlines, &jaqUseFontOutlines, &_UseFontOutlines) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't create '%s'.\n", "wglUseFontOutlines");
		return false;
	}
	else
	{
		touchstone::windows::register_hook("wglUseFontOutlines", _UseFontOutlines);
	}

	if (MH_EnableHook(&wglUseFontOutlines) != MH_OK)
	{
		std::fprintf(stderr, "Couldn't enable '%s'.\n", "wglUseFontOutlines");
		return false;
	}

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
