#!/usr/bin/env moon
-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

Config = {
	-- Maximum duration before the current path is considered finished.
	max_duration: 20

	-- Minimum path length, in pixels. Any paths shorter are pruned.
	min_path_length: 5
}

Args = { ... }
if #Args < 1
	io.stdout\write("process_path <input>", "\n")
	os.exit(1)

Samples = {}
do
	file, message = io.open(Args[1])
	if not file
		io.stderr\write("couldn't open '#{Args[1]}': #{message}", "\n")
		os.exit(1)

	pattern = "%s*(%w+)%s*,%s*(%d+)%s*,%s*(%d+)%s*,%s*(%d+)%s*"
	for line in file\lines!
		classification, x, y, timestamp = line\match(pattern)
		if not classification
			continue

		x = tonumber(x)
		y = tonumber(y)
		timestamp = tonumber(timestamp)

		sample = { :classification, :x, :y, :timestamp }
		table.insert(Samples, sample)

Paths = {}
do
	current_path = {}
	add_path = ->
		if #current_path < 2
			return

		start = current_path[1]
		stop = current_path[#current_path]

		dx = stop.x - start.x
		dy = stop.y - start.y
		d = dx * dx + dy * dy
		r = Config.min_path_length * Config.min_path_length
		if d < r
			return

		current_path.length = math.sqrt(d)
		current_path.direction = math.atan2(dy, dx)
		if current_path.direction < 0
			current_path.direction += math.pi * 2

		for i = 2, #current_path
			current_path[i].x -= start.x
			current_path[i].y -= start.y
		start.x = 0
		start.y = 0

		table.insert(Paths, current_path)
		current_path = {}
	for i = 2, #Samples
		previous_sample = Samples[i - 1]
		current_sample = Samples[i]

		if current_sample.classification == 'click'
			add_path!
			continue

		if current_sample.timestamp > Config.max_duration
			add_path!
			continue

		table.insert(current_path, current_sample)

pretty_print = (format, ...) ->
	print(string.format(format, ...))

pretty_print("# @path <length [pixels]> <direction [degrees]>")
pretty_print("# <x [pixels]> <y [pixels]> <period [milliseconds]> [...]")
pretty_print("# processed %d paths total", #Paths)
for i = 1, #Paths
	path = Paths[i]
	pretty_print("@path %d %0.02f", path.length, path.direction * (180 / math.pi))
	for j = 1, #path
		pretty_print("%d %d %d", path[j].x, path[j].y, path[j].timestamp)
