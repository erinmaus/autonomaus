-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Log = require "maus.common.Log"
Paley = require "paley.Paley"
Native = require "eiznebahn.detail.Native"
KeySequence = require "eiznebahn.keyboard.KeySequence"
MousePathCollection = require "eiznebahn.mouse.MousePathCollection"

roll = (min, max, rolls) ->
	duration = 0
	for i = 1, rolls
		duration += math.random(min, max)

	return duration

class Application
	new: (data) =>
		@client = Paley.Client("EIZNEBAHN")
		@environment = {
			address: data.address
			port: data.port
		}
		@frame = {
			x: data.frame.width
			y: data.frame.height
		}
		@is_running = false

		@input_state = Native.rv_create_input_event_state(nil)
		if data.resize_window
			@window = Native.rv_select_window!
			@\resize_window(
				data.resize_dimensions.width,
				data.resize_dimensions.height)
		else
			@window = false

		@mouse_paths = MousePathCollection!
		for i = 1, #data.mouse_paths
			@mouse_paths\load(data.mouse_paths[i])

		@virtual = false
		@gesture = false
		@gesture_failed = false
		@sequence = false

	assign_window: (window) =>
		if @window
			Native.rv_destroy_window(@window)
		@window = window
		if @input_state
			Native.rv_destroy_input_event_state(@input_state)
		@input_state = Native.rv_create_input_event_state(@window)

	resize_window: (width, height) =>
		if @window != nil
			Native.rv_resize_window(@window, width, height)

	connect: =>
		connected = @client\connect(@environment.address, @environment.port)
		if not connected
			Log\error("couldn't connect to RESIST server at #{@environment.address}:#{@environment.port}")
			os.exit(1)
		
		Log\info('high', "connected to RESIST server")

	process_resist_event: (event) =>
		switch event.name
			when 'cease'
				Log\info('high', "received quit event; terminating...")
				@is_running = false
			else
				Log\info('high',
					"received event 'RESIST.#{event.name}', didn't process")

	process_arden_event: (event) =>
		switch event.name
			when 'draw_current_window'
				Log\info('high', "updating current window")
				@\assign_window(Native.rv_window_from_handle(event.root.handle))

	process_event: (event) =>
		switch event.name
			when 'virtual_gesture'
				@\process_virtual_gesture(event)
			when 'begin_gesture'
				@\process_begin_gesture(event)
			when 'end_gesture'
				@\process_end_gesture(event)
			when 'mouse_move'
				@\process_mouse_move(event)
			when 'mouse_button_press'
				@\process_mouse_button_press(event)
			when 'mouse_button_release'
				@\process_mouse_button_release(event)
			when 'mouse_scroll_up'
				@\process_mouse_scroll_up(event)
			when 'mouse_scroll_down'
				@\process_mouse_scroll_down(event)
			when 'begin_sequence'
				@\process_begin_sequence(event)
			when 'end_sequence'
				@\process_end_sequence(event)
			when 'key'
				@\process_key(event)

	process_virtual_gesture: (event) =>
		if event.root.virtual == true
			@virtual = true
		elseif event.root.virtual == false
			@virtual = false

	process_begin_gesture: (event) =>
		if @gesture != false
			Log\error("gesture already in progress; dropping token %d", event.root.token)
			return

		@gesture = event.root.token
		@gesture_failed = false

		Native.rv_begin_event_sequence(@input_state)

	process_end_gesture: (event) =>
		if @gesture == false
			Log\error("gesture not in progress")
			return

		if @gesture != event.root.token
			Log\error("gesture token bad: expected %d, got %d", @gesture, event.root.token)
			return

		local event
		if @gesture_failed
			event = Paley.Event("gesture_failed")
		else
			event = Paley.Event("gesture_completed")
			Native.rv_end_event_sequence(@input_state)
		event.root.token = @gesture

		@client\send(event)

		@gesture = false
		@gesture_failed = false

	process_mouse_move: (event) =>
		if not @gesture or @gesture_failed
			return

		current_x = Native.rv_get_mouse_x(@input_state)
		current_y = Native.rv_get_mouse_y(@input_state)

		local x, y
		if event.root.relative
			Log\info('high', "processing relative move")
			x = current_x + event.root.x
			y = current_y + event.root.y
		else
			Log\info('high', "processing absolute move")
			x = Native.rv_get_window_x(@window) + event.root.x - @frame.x
			y = Native.rv_get_window_y(@window) + event.root.y - @frame.y

		path = @mouse_paths\find(current_x, current_y, x, y)
		if not path
			Log\error("couldn't move to (%d, %d): no viable path found", x, y)
			@gesture_failed = true if @gesture != false
			return

		Log\info('high',
			"moving mouse to local position (%d, %d) [global: (%d, %d)]",
			event.root.x, event.root.y, x, y)
		if @virtual
			valid = path\validate(
				current_x, current_y,
				Native.rv_get_window_x(@window),
				Native.rv_get_window_y(@window),
				Native.rv_get_window_width(@window),
				Native.rv_get_window_height(@window))
			if not valid
				Log\info('high', "virtual move failed")
				@gesture_failed = true
		else if not @virtual
			path\replay(current_x, current_y, @input_state)

	process_mouse_button_press: (event) =>
		if not @gesture or @gesture_failed or @virtual
			return

		Native.rv_send_mouse_button_down(
			@input_state, event.root.button, 0)

	process_mouse_button_release: (event) =>
		if not @gesture or @gesture_failed or @virtual
			return

		Native.rv_send_mouse_button_up(
			@input_state, event.root.button, roll(10, 15, 5))

	process_mouse_scroll_up: (event) =>
		if not @gesture or @gesture_failed or @virtual
			return

		Native.rv_send_mouse_button_down(@input_state, 4, 0)
		Native.rv_send_mouse_button_up(@input_state, 4, roll(5, 10, 15))

	process_mouse_scroll_down: (event) =>
		if not @gesture or @gesture_failed or @virtual
			return

		Native.rv_send_mouse_button_down(@input_state, 5, 0)
		Native.rv_send_mouse_button_up(@input_state, 5, roll(5, 10, 15))

	process_begin_sequence: (event) =>
		if @sequence != false
			Log\error("sequence already in progress; dropping token %d", event.root.token)
			return

		@sequence = KeySequence(event.root.token)

	process_end_sequence: (event) =>
		if @sequence == false
			Log\error("sequence not in progress")
			return

		if @sequence.token != event.root.token
			Log\error("sequence token bad; expected %d, got %d", @sequence.token, event.root.token)
			return

		Log\info('high', "pressing key sequence...")
		@sequence\replay(@input_state)
		Log\info('high', "done.")
		event = Paley.Event("sequence_completed")
		event.root.token = @sequence.token
		@client\send(event)

		@sequence = false

	process_key: (event) =>
		if not @sequence
			return

		@sequence\add_key(event.root.key, event.root.modifiers)

	send_mouse_position: (x, y) =>
		if not @window
			return

		event = Paley.Event("mouse_position")
		event.root.x = x - Native.rv_get_window_x(@window)
		event.root.y = y - Native.rv_get_window_y(@window)

		@client\send(event)

	run: =>
		@\connect!

		current_x = Native.rv_get_mouse_x(@input_state)
		current_y = Native.rv_get_mouse_y(@input_state)
		@is_running = true
		while @is_running
			scope, name, event = @client\receive!
			while scope
				if scope == "RESIST"
					@\process_resist_event(event)
				elseif scope == "ARDEN"
					@\process_arden_event(event)
				elseif scope == "EIZNEBAHN"
					@\process_event(event)
				scope, name, event = @client\receive!
			do
				x = Native.rv_get_mouse_x(@input_state)
				y = Native.rv_get_mouse_y(@input_state)
				if current_x != x or current_y != y
					--@\send_mouse_position(x, y)
					current_x = x
					current_y = y
			Paley.sleep(1)

		Log\info('high', "terminated")

return Application
