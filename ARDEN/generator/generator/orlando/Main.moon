-- This file is a part of ARDEN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Generator = require "generator.common.Generator"

generator = Generator(true)
generator\include_version("GL_VERSION_1_0")
generator\include_version("GL_VERSION_1_1")
generator\include_version("GL_CORE_3_0")
generator\add("templates/orlando/hooks.cpp.tpl", "../orlando/source/hooks.cpp")

if not generator\set_platform("windows")
	io.stderr\write("couldn't select platform windows; output code probably broken\n")

generator\generate!
