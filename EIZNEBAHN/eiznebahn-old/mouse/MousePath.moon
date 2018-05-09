-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Native = require "eiznebahn.detail.Native"
Matrix = require "maus.math.Matrix"
Vector = require "maus.math.Vector"

class MousePath
	new: (length, direction) =>
		@length = length
		@direction = direction
		@samples = {}

	parse_sample: (line) =>
		pattern = "%s*([-%d]%d*)%s*([-%d]%d*)%s*(%d+)"
		x, y, delta = line\match(pattern)
		if x and y and delta
			x = tonumber(x)
			y = tonumber(y)
			delta = tonumber(delta)

			table.insert(@samples, { :x, :y, :delta })

	transform: (new_length, new_direction) =>
		result = MousePath(new_length, new_direction)

		local matrix
		do
			angle = new_direction - @direction
			if angle < 0
				angle += math.pi * 2
			rotation = Matrix.rotate(Vector(0, 0, 1), angle)
			scale = Matrix.scale(Vector(new_length / @length))
			matrix = rotation * scale

		for _, sample in ipairs(@samples)
			position = Vector(sample.x, sample.y, 0)
			position = matrix\transform(position)

			x = math.floor(position.x + 0.5)
			y = math.floor(position.y + 0.5)
			delta = sample.delta * (new_length / @length)
			table.insert(result.samples, { :x, :y, :delta })

		return result

	replay: (start_x, start_y, input_state) =>
		for _, sample in ipairs(@samples)
			x = start_x + sample.x
			y = start_y + sample.y
			Native.rv_send_mouse_move(input_state, x, y, sample.delta)

	validate: (start_x, start_y, window_x, window_y, window_width, window_height) =>
		for _, sample in ipairs(@samples)
			x = start_x + sample.x
			y = start_y + sample.y

			match_x = x < window_x or x > window_x + window_width
			match_y = y < window_y or y > window_y + window_height

			if match_x or match_y
				return false
		return true

return MousePath

