-- This file is a part of ARDEN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Generator = require "generator.common.Generator"

generator = Generator!
generator\add("templates/touchstone/gl_calls.hpp.inl.tpl", "../touchstone/include/touchstone/gl/calls.hpp.inl")
generator\add("templates/touchstone/gl_dispatch.hpp.inl.tpl", "../touchstone/include/touchstone/gl/dispatch.hpp.inl")
generator\add("templates/touchstone/gl_calls.cpp.tpl", "../touchstone/source/gl/calls.cpp")
generator\add("templates/touchstone/gl_enums.hpp.tpl", "../touchstone/include/touchstone/gl/enums.hpp")
generator\add("templates/touchstone/gl_immediateDispatch.hpp.inl.tpl", "../touchstone/include/touchstone/gl/immediateDispatch.hpp.inl")
generator\add("templates/touchstone/gl_immediateDispatch.cpp.tpl", "../touchstone/source/gl/immediateDispatch.cpp")
generator\add("templates/touchstone/gl_glTrace.cpp.inl.tpl", "../touchstone/source/gl/glTrace.cpp.inl")
generator\add("templates/touchstone/gl_traceBuilder.hpp.inl.tpl", "../touchstone/include/touchstone/gl/traceBuilder.hpp.inl")
generator\add("templates/touchstone/gl_traceBuilder.cpp.inl.tpl", "../touchstone/source/gl/traceBuilder.cpp.inl")

do
	platform = select(1, ...)
	if platform == nil
		io.stderr\write("no platform selected; output code probably broken\n")
	elseif not generator\set_platform(platform)
		io.stderr\write("couldn't select platform #{platform}; output code probably broken\n")

generator\generate!
