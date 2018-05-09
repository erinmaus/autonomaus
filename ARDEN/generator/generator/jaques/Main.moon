-- This file is a part of ARDEN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Generator = require "generator.common.Generator"

generator = Generator!
generator\add("templates/jaques/hooks.cpp.tpl", "../jaques/source/hooks.cpp")

do
	platform = select(1, ...)
	if platform == nil
		io.stderr\write("no platform selected; output code probably broken\n")
	elseif not generator\set_platform(platform)
		io.stderr\write("couldn't select platform #{platform}; output code probably broken\n")

generator\add_exception("glXGetProcAddress")
generator\add_exception("glXGetProcAddressARB")
generator\add_exception("glXSwapBuffers")
generator\add_exception("glXCreateWindow")
generator\add_exception("glXDestroyWindow")
generator\add_exception("glXMakeCurrent")
generator\add_exception("glXMakeContextCurrent")
generator\add_exception("wglCreateContext")
generator\add_exception("wglDeleteContext")
generator\add_exception("wglMakeCurrent")
generator\add_exception("wglGetProcAddress")

generator\generate!
