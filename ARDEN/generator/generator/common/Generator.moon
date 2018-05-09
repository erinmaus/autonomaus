-- This file is a part of ARDEN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

GLFunctions = require "generator.GLFunctions"
Template = require "generator.WTemplate"

class Generator
	new: (skip) =>
		@templates = {}

		@spec = GLFunctions!
		@exceptions = {}

		@versions = {}
		if not skip
			@\include_version("GL_VERSION_1_0")
			@\include_version("GL_VERSION_1_1")
			@\include_version("GL_VERSION_1_2")
			@\include_version("GL_VERSION_1_3")
			@\include_version("GL_VERSION_1_4")
			@\include_version("GL_VERSION_1_5")
			@\include_version("GL_VERSION_2_0")
			@\include_version("GL_VERSION_2_1")
			@\include_version("GL_VERSION_3_0")
			@\include_version("GL_VERSION_3_1")
			@\include_version("GL_VERSION_3_2")
			@\include_version("GL_VERSION_3_3")

		@extensions = {}
		if not skip
			@\include_extension("GL_EXT_framebuffer_object")
			@\include_extension("GL_EXT_texture_filter_anisotropic")
			@\include_extension("GL_ARB_copy_image")
			@\include_extension("GL_EXT_texture_compression_dxt1")
			@\include_extension("GL_EXT_texture_compression_s3tc")
			@\include_extension("GL_EXT_texture_sRGB")
			@\include_extension("GL_ARB_invalidate_subdata")
			@\include_extension("GL_EXT_framebuffer_blit")

		@platform_spec = false

	add: (input, output) =>
		@templates[output] = Template(input)

	set_platform: (platform) =>
		switch platform
			when 'posix'
				@platform_spec = "GLX"
			when 'windows'
				@platform_spec = "WGL"
			else
				return false
		return true

	add_exception: (exception) =>
		@exceptions[exception] = true

	remove_exception: (exception) =>
		@exceptions[exception] = true

	include_version: (version) =>
		if not @versions[version]
			@versions[version] = true
			table.insert(@versions, version)

	exclude_version: (version) =>
		if @versions[version]
			@versions[version] = nil
			for i = 1, #@versions
				if @versions[i] == version
					table.remove(@versions, i)
					break

	include_extension: (extension) =>
		if not @extensions[extension]
			@extensions[extension] = true
			table.insert(@extensions, extension)

	exclude_extension: (version) =>
		if @extension[version]
			@extension[version] = nil
			for i = 1, #@extension
				if @extension[i] == version
					table.remove(@extension, i)
					break

	generate: =>
		@spec\load_version("spec/gl.xml", @versions, @extensions)
		if @platform_spec
			filename = "spec/" .. @platform_spec\lower! .. ".xml"
			@spec\load(filename, { @platform_spec })

		index = 1
		while index <= #@spec.funcs
			func = @spec.funcs[index]
			if @exceptions[func.name] == true
				table.remove(@spec.funcs, index)
			else
				index += 1

		compare_func = (a, b) ->
			return a.name < b.name
		table.sort(@spec.funcs, compare_func)

		for i = 1, #@spec.funcs
			func = @spec.funcs[i]
			func.brief_name = func.name\match("^gl(.+)") or func.name\match("^wgl(.+)") or func.name
			func.is_glx = func.name\match("^glX(.+)") != nil
			func.is_wgl = func.name\match("^wgl(.+)") != nil
			func.is_deserializable = not func.is_glx and not func.is_wgl

			for i = 1, #func.parameters.types
				parameter = {}
				parameter.type = func.parameters.types[i]
				parameter.name = func.parameters.names[i]
				parameter.arden_formatted_name = parameter.name\gsub("[A-Z]", (a) -> "_" .. a\lower!)
				parameter.is_const = parameter.type\match("^const") != nil
				parameter.mutable_type = parameter.type\gsub("const", "")\gsub("%s", "")
				parameter.is_pointer = parameter.mutable_type\match("%*$") != nil
				parameter.is_pointer_array = parameter.mutable_type\match("%*%*$") != nil

				if parameter.is_pointer
					parameter.primitive_type = parameter.mutable_type\gsub("%*", "")
					parameter.element_type = parameter.mutable_type\sub(1, -2)

				func.parameters[i] = parameter

			for i = 1, #func.data
				data = func.data[i]
				parameter = func.parameters[data.index]
				parameter.data = data

				if parameter.is_pointer_array and data.count
					parameter.is_pointer_array = false

		compare_enum = (a, b) ->
			enum_to_number = (e) ->
				hex = e.value\match("0x(%x+)")
				if hex != nil
					return tonumber(hex, 16)
				return tonumber(e.value)
			i = enum_to_number(a)
			j = enum_to_number(b)

			if i == j
				return a.name < b.name
			return i < j
		table.sort(@spec.enums, compare_enum)

		for filename, template in pairs(@templates)
			template\build(@spec, filename)
			print("generated '#{filename}'")

return Generator
