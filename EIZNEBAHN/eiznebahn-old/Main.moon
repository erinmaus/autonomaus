-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

ParameterParser = require "maus.common.ParameterParser"
Application = require "eiznebahn.Application"

AppState = {
	arguments: {
		address: os.getenv("RESIST_ADDRESS") or "127.0.0.1"
		port: tonumber(os.getenv("RESIST_PORT") or "18176")
		mode: 'default'
		resize_window: false
		mouse_paths: {}
		resize_dimensions: { width: 1680, height: 960 }
		frame: { width: 0, height: 0 }
	}
}

AppState.set_address = (value) ->
	AppState.arguments.address = value

AppState.set_port = (value) ->
	AppState.arguments.port = tonumber(value)

AppState.set_enable_resize_window = (value) ->
	AppState.arguments.resize_window = true
	width, height = value\match("%s*(%d+)%s*,%s*(%d+)")
	if width and height
		AppState.arguments.resize_dimensions.width = tonumber(width)
		AppState.arguments.resize_dimensions.height = tonumber(height)

AppState.set_frame = (value) ->
	width, height = value\match("%s*(.+)%s*,%s*(.+)")
	if width and height
		AppState.arguments.frame.width = tonumber(width) or 0
		AppState.arguments.frame.height = tonumber(height) or 0

AppState.set_listen_mode = (value) ->
	AppState.arguments.mode = 'listen'
	AppState.arguments.output_filename = value

AppState.add_mouse_path = (value) ->
	table.insert(AppState.arguments.mouse_paths, value)

Parameters = ParameterParser("walden")
do
	Parameters\add(
		"address", "a",
		"Connect to address <ARG>. Defaults to #{AppState.arguments.address}",
		true,
		AppState.set_address)

	Parameters\add(
		"port", "p",
		"Connect to port <ARG>. Defaults to #{AppState.arguments.port}",
		true,
		AppState.set_port)

	Parameters\add(
		"resize-window", "r",
		{
			"Resize window to <ARG> pixels."
			"Argument should be in format WIDTH,HEIGHT."
		},
		true,
		AppState.set_enable_resize_window)

	Parameters\add(
		"frame", "f",
		{
			"Setting frame padding."
			"Argument should be in format WIDTH,HEIGHT."
		},
		true,
		AppState.set_frame)

	Parameters\add(
		"add-mouse-paths", "m",
		"Adds for processed mouse paths from file <ARG>.",
		true,
		AppState.add_mouse_path)

	Parameters\add(
		"listen", "l",
		"Listens for mouse events and write them to file <ARG>.",
		true,
		AppState.set_listen_mode)

	print_usage = ->
		Parameters\print_usage!
		os.exit(0)

	Parameters\add(
		"help", "h",
		"Prints usage information.",
		false,
		print_usage)

	success, message = Parameters\parse({ ... })
	if not success
		io.stderr\write(message, "\n")
		os.exit(1)

switch AppState.arguments.mode
	when 'listen'
		MouseListener = require "eiznebahn.mouse.MouseListener"
		listener = MouseListener(AppState.arguments.output_filename)
		listener\run!
	when 'default'
		app = Application(AppState.arguments)
		do
			math.randomseed(os.time())
			app\run!
