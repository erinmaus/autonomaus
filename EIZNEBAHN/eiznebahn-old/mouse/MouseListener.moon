-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Native = require "eiznebahn.detail.Native"
Log = require "maus.common.Log"

class MouseListener
	new: (filename) =>
		@filename = filename
		@event_state = Native.rv_create_input_event_state(nil)
		@current_x = 0
		@current_y = 0
		@current_button_state = 0
		@previous_time = 0
		@current_time = 0

	write: (classification) =>
		file, message = io.open(@filename, 'a')
		if not file
			Log\error("couldn't open '%s': %s", @filename, message)
			return

		s = string.format(
			"%s, %d, %d, %d",
			classification,
			@current_x, @current_y,
			@current_time - @previous_time)
		file\write(s, "\n")
		file\close!

	run: =>
		Log\info('high', "listening for mouse events")
		@current_x = Native.rv_get_mouse_x(@event_state)
		@current_y = Native.rv_get_mouse_y(@event_state)
		@current_button_state = Native.rv_get_mouse_buttons(@event_state)
		@previous_time = tonumber(Native.rv_get_time())
		@current_time = @previous_time

		while true
			x = Native.rv_get_mouse_x(@event_state)
			y = Native.rv_get_mouse_y(@event_state)
			button_state = Native.rv_get_mouse_buttons(@event_state)

			same_position = @current_x == x and @current_y == y
			same_buttons = @current_button_state == button_state

			if same_position and same_buttons
				Native.rv_sleep(1)
				continue

			@previous_time = @current_time
			@current_time = tonumber(Native.rv_get_time())

			if not same_position
				@current_x = x
				@current_y = y
				@\write("move")
			if not same_buttons
				@current_button_state = button_state
				@\write("click")

return MouseListener
