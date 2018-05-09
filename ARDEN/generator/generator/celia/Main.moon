-- This file is a part of ARDEN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

GLFunctions = require "generator.GLFunctions"
Template = require "generator.Template"
Exceptions = require "generator.celia.Exceptions"

Templates = {
	forwarder_func: Template("templates/celia/forwarder_func.cpp.tpl")
	client_func: Template("templates/celia/client_func.cpp.tpl")
	forwarder: Template("templates/celia/forwarder.cpp.tpl")
	client_header: Template("templates/celia/client.hpp.tpl")
	client_source: Template("templates/celia/client.cpp.tpl")
}

Versions = {
	"GL_VERSION_1_0",
	"GL_VERSION_1_1",
	"GL_VERSION_1_2",
	"GL_VERSION_1_3",
	"GL_VERSION_1_4",
	"GL_VERSION_1_5",
	"GL_VERSION_2_0",
	"GL_VERSION_2_1",
	"GL_VERSION_3_0",
	"GL_VERSION_3_1",
	"GL_VERSION_3_2",
	"GL_VERSION_3_3"
}
Extensions = { "GL_EXT_framebuffer_object" }
Spec = GLFunctions!
Spec\load_version("spec/gl.xml", Versions, Extensions)

client = { funcs: {} }
forwarder = { funcs: {} }
for i = 1, #Spec.funcs
	if Exceptions[Spec.funcs[i].name]
		continue

	table.insert(forwarder.funcs, Templates.forwarder_func\build({ func: Spec.funcs[i] }))
	table.insert(client.funcs, Templates.client_func\build({ func: Spec.funcs[i] }))

Templates.forwarder\build(forwarder, "../celia/source/forwarder/defaults.cpp")
Templates.client_header\build(Spec, "../celia/include/celia/client/functions.hpp")
Templates.client_source\build(client, "../celia/source/client/defaults.cpp")
