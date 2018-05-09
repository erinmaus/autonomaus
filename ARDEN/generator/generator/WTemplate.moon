-- This file is a part of ARDEN.
--
-- Look, but don't touch.
--
-- Copyright 2016 [bk]door.maus

template = require "template"
bail = (message) ->
	io.stderr\write(message, "\n")
	os.exit(1)

class Template
	new: (filename) =>
		if filename then
			@\from_file(filename)
		else
			@_template = ->

	from_file: (filename) =>
		file, message = io.open(filename, 'r')
		if not file then
			bail(message)

		source = file\read("*a")
		file\close!

		@\from_string(source)

	from_string: (source) =>
		result, message = loadstring(template.parse(source))
		if not result
			bail(message)

		@_template = result!

	build: (args, filename) =>
		result = ""
		callback = (...) ->
			args = { ... }
			n = table.maxn(args)
			for i = 1, n
				result ..= tostring(args[n])

		template.print(@_template, args, callback)

		if filename
			file, message = io.open(filename, 'w')
			if not file then
				bail(message)

			file\write(result)
			file\close!

		return result

return Template
