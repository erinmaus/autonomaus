-- This file is a part of ARDEN.
--
-- Look, but don't touch.
--
-- Copyright 2016 [bk]door.maus

lub = require "lub"
xml = require "xml"

class Spec
	new: (filename) =>
		@_spec = xml.loadpath(filename)

	find: (tag, parent, current) =>
		parent = parent or @_spec

		local found_current
		if current
			found_current = false
		else
			found_current = true

		do_find = (node) ->
			if not found_current
				if node == current
					found_current = true
			elseif node.xml == tag then
				return node

			return nil

		return lub.search(parent, do_find)

	get_tag: (element) =>
		if type(element) == "table"
			return element.xml
		return nil

trim_whitespace = (s) ->
	return s\gsub("^%s*(.-)%s*$", "%1")

class GLFunctions
	new: =>
		@collected_funcs = {}
		@collected_enums = {}

		@funcs = {}
		@_imported_funcs = {}
		@enums = {}
		@_imported_enums = {}

	_collect_func: (spec, func) =>
		r = {
			name: "undefined"
			result: ""
			parameters: { names: {}, types: {}, full: {} }
			data: {}
		}

		proto = spec\find("proto", func)
		for i = 1, #proto
			tag = spec\get_tag(proto[i])
			switch tag
				when "name" then r.name = proto[i][1]
				when "ptype" then r.result ..= proto[i][1]
				when nil then r.result ..= proto[i]
		r.result = trim_whitespace(r.result)
		r.name = trim_whitespace(r.name)

		parameter = spec\find("param", func)
		while parameter != nil
			p = { type: "", name: "argument" }
			if parameter["len"] != nil
				data_parameters = parameter["len"]\match("COMPSIZE%((.*)%)")

				d = { index: #r.parameters.full + 1 }
				if parameter["len"] == "COMPSIZE()"
					d.dependent = true
				if data_parameters != nil
					d.components = [m for m in data_parameters\gmatch("([%w_]*)") when m != ""]
				else
					d.count = parameter["len"]
				table.insert(r.data, d)

			for i = 1, #parameter
				tag = spec\get_tag(parameter[i])
				switch tag
					when "ptype" then p.type ..= parameter[i][1]
					when "name" then p.name = parameter[i][1]
					when nil then p.type ..= parameter[i]

			p.type = trim_whitespace(p.type)
			p.name = trim_whitespace(p.name)

			table.insert(r.parameters.names, p.name)
			table.insert(r.parameters.types, p.type)
			table.insert(r.parameters.full, "#{p.type} #{p.name}")

			parameter = spec\find("param", func, parameter)

		@collected_funcs[r.name] = r

	_collect_funcs: (spec) =>
		commands = spec\find("commands")
		while commands != nil
			for i = 1, #commands
				tag = spec\get_tag(commands[i])
				if tag == "command"
					@\_collect_func(spec, commands[i])
			commands = spec\find("commands", nil, commands)

	_collect_enum: (spec, enum) =>
		r = {
			value: enum["value"]
			name: enum["name"]
		}

		@collected_enums[r.name] = r

	_collect_enums: (spec) =>
		enums = spec\find("enums")
		while enums != nil
			for i = 1, #enums
				tag = spec\get_tag(enums[i])
				if tag == "enum"
					@\_collect_enum(spec, enums[i])
			enums = spec\find("enums", nil, enums)

	_import_enum: (name) =>
		enum = @collected_enums[name]
		if enum and not @_imported_enums[name]
			table.insert(@enums, enum)
			@_imported_enums[name] = true

	_import_func: (name) =>
		func = @collected_funcs[name]
		if func and not @_imported_funcs[name]
			table.insert(@funcs, func)
			@_imported_funcs[name] = true

	_import: (spec, parent, tag, filter) =>
		element = spec\find(tag, parent)
		while element != nil
			if filter(element)
				requires = spec\find("require", element)
				while requires != nil
					for i = 1, #requires
						switch spec\get_tag(requires[i])
							when "enum"
								@\_import_enum(requires[i]["name"])
							when "command"
								@\_import_func(requires[i]["name"])
					requires = spec\find("require", element, requires)
			element = spec\find(tag, parent, element)

	_purge_enum: (name) =>
		enum = @collected_enums[name]
		if @_imported_enums[name] and enum != nil
			index = 1
			while index <= #@enums
				if @enums[index] == enum
					table.remove(@enums, index)
					break
				index += 1
			@_imported_enums[name] = nil

	_purge_func: (name) =>
		enum = @collected_funcs[name]
		if @_imported_funcs[name] and enum != nil
			index = 1
			while index <= #@funcs
				if @funcs[index] == enum
					table.remove(@funcs, index)
					break
				index += 1
			@_imported_funcs[name] = nil

	_purge: (spec, parent, tag, filter) =>
		element = spec\find(tag, parent)
		while element != nil
			if filter(element)
				removes = spec\find("remove", element)
				while removes != nil
					for i = 1, #removes
						switch spec\get_tag(removes[i])
							when "enum"
								@\_purge_enum(removes[i]["name"])
							when "command"
								@\_purge_func(removes[i]["name"])
					removes = spec\find("remove", element, removes)
			element = spec\find(tag, parent, element)

	load: (filename, extensions) =>
		extensions = extensions or {}
		extensions_filter = (element) ->
			for i = 1, #extensions
				if element["name"]\match("(#{extensions[i]})")
					return true
			return false

		feature_filter = (element) ->
			return element["api"] == "gl"

		spec = Spec(filename)

		@\_collect_funcs(spec)
		@\_collect_enums(spec)

		@\_import(spec, nil, "feature", feature_filter)
		@\_import(spec, spec\find("extensions"), "extension", extensions_filter)

	load_version: (filename, versions, extensions) =>
		extensions = extensions or {}
		extensions_filter = (element) ->
			for i = 1, #extensions
				if element["name"]\match("(#{extensions[i]})")
					return true
			return false

		version_filter = (element) ->
			if element["api"] != "gl"
				return false
			for i = 1, #versions
				if element["name"] == versions[i]
					return true
			return false

		spec = Spec(filename)

		@\_collect_funcs(spec)
		@\_collect_enums(spec)

		@\_import(spec, nil, "feature", version_filter)
		@\_import(spec, spec\find("extensions"), "extension", extensions_filter)

		@\_purge(spec, nil, "feature", version_filter)

return GLFunctions
