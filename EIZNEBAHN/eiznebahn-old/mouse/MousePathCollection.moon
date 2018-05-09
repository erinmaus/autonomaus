-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Log = require "maus.common.Log"
Native = require "eiznebahn.detail.Native"
MousePath = require "eiznebahn.mouse.MousePath"
RNG = require "maus.math.RNG"

class MousePathCollection
	new: (input_state) =>
		@paths = {}
		@input_state = input_state or Native.rv_create_input_event_state(nil)
		@rng = RNG!
		@path_threshold = 20

	load: (filename) =>
		file, message = io.open(filename)
		if not file
			Log\error("couldn't open file '%s': %s", filename, message)
			return false

		comment_pattern = "%s*#%s.*(.*)"
		path_pattern = "@path%s*(%d+)%s*(%d+%.%d+)"

		local current_path
		for line in file\lines!
			if line\match(comment_pattern)
				continue

			do
				length, direction = line\match(path_pattern)
				if length and direction
					length = tonumber(length)
					direction = tonumber(direction)
					direction *= (math.pi / 180)

					table.insert(@paths, current_path) if current_path != nil
					current_path = MousePath(length, direction)
					continue

			if current_path != nil
				current_path\parse_sample(line)
		file\close!

		table.sort(@paths, (a, b) -> a.length < b.length)

		return true

	find: (current_x, current_y, target_x, target_y) =>
		local length, direction
		do
			dx = target_x - current_x
			dy = target_y - current_y
			length = math.sqrt(dx * dx + dy * dy)
			direction = math.atan2(dy, dx)
			if direction < 0
				direction += math.pi * 2

		best = {}
		get_metric = (field, value, delta) ->
			return (path) ->math.abs(value / path[field] - 1) < delta
		collect_best = (...) ->
			metrics = { ... }
			for _, path in ipairs(@paths)
				if #best >= @path_threshold
					return
				fit = true
				for i = 1, #metrics
					if not metrics[i](path)
						fit = false
						break
				table.insert(best, path) if fit

		collect_best(
			get_metric("length", length, 0.1),
			get_metric("direction", direction, 0.1))
		collect_best(
			get_metric("length", length, 0.5),
			get_metric("direction", direction, 0.25))
		collect_best(get_metric("length", length, 1.0))

		if #best == 0
			return nil
		if #best == 1
			return best\transform(length, direction)

		result = best[@rng\next(#best)]
		return result\transform(length, direction)

return MousePathCollection
