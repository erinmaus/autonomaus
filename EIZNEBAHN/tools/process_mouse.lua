#!/usr/bin/env lua

-- This file is a part of EIZNEBAHN.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local file, message = io.open(select(1, ...) or "<FILENAME>", "r")
if not file then
	io.stderr:write(message, "\n")
	return 1
end

local frames = {}
local parsing = true
local current_line = file:read("*l")
local previous_time = 0
while current_line ~= nil do
	local index, time = current_line:match("frame (%d+) (%d+)")
	if index and time then
		local frame = {}
		frame.duration = tonumber(time) - previous_time
		frame.position = { x = 0, y = 0 }
		frame.buttons = {}
		frame.widgets = {}

		previous_time = tonumber(time)

		local line = file:read("*l")
		while line ~= nil do
			local command, arguments = line:match("(%w+)(.*)")
			if command == "frame" then
				break
			end

			if command == "position" then
				local x, y = arguments:match("(%d+) (%d+)")
				frame.position.x = tonumber(x)
				frame.position.y = tonumber(y)
			elseif command == "widget" then
				local x, y, width, height, class, name = arguments:match("(%d+) (%d+) (%d+) (%d+) ([^%s]+) (^%s]+)")
				if x then
					local widget = {
						x = tonumber(x),
						y = tonumber(y),
						width = tonumber(width),
						height = tonumber(height),
						class = class
					}
					table.insert(frame.widgets, widget)
				end
			elseif command == "pressed" then
				local button = tonumber(arguments)
				if button then
					frame.buttons[button] = true
				end
			elseif command == "session" then
				break
			end
			
			line = file:read("*l")
		end

		current_line = line
		table.insert(frames, frame)
	else
		if current_line:match("session") then
			previous_time = 0
		end

		current_line = file:read("*l")
	end
end

local Conf = {
	max_idle_duration = 80,
	min_path_length = 8
}

local paths = {}
local current_path = {}
local start_x , start_y = 0, 0
local current_x, current_y = frames[1].position.x, frames[1].position.y
local last_movement = 0
for i = 1, #frames do
	local sample = {
		x = frames[i].position.x,
		y = frames[i].position.y,
		duration = frames[i].duration
	}

	local is_click
	if next(frames[i].buttons, nil) ~= nil then
		is_click = true
	else
		last_movement = last_movement + frames[i].duration
		is_click = false
	end

	if last_movement > Conf.max_idle_duration or is_click then
		if #current_path > 0 then
			table.insert(paths, current_path)
			current_path = {}
		end

		last_movement = 0
	else
		if current_x ~= sample.x or current_y ~= sample.y then
			current_x = sample.x
			current_y = sample.y
			last_movement = 0

			if #current_path == 0 then
				start_x = sample.x
				start_y = sample.y
				table.insert(current_path, { x = 0, y = 0, duration = sample.duration })
			else
				sample.x = sample.x - start_x
				sample.y = sample.y - start_y
				table.insert(current_path, sample)
			end
		end
	end
end

function path_length(p)
	local x = p[1].x - p[#p].x
	local y = p[1].y - p[#p].y
	return math.sqrt(x ^ 2 + y ^ 2)
end

function compare_length(a, b)
	local a_length = path_length(a)
	local b_length = path_length(b)
	return a_length < b_length
end

table.sort(paths, compare_length)

local output_paths, message = io.open("paths.dat", "w")
if not output_paths then
	io.stderr:write(message, "\n")
else
	for i = 1, #paths do
		if path_length(paths[i]) > Conf.min_path_length then
			output_paths:write("path", "\n")
			local path = paths[i]
			for j = 1, #path do
				output_paths:write("sample", " ")
				output_paths:write(path[j].x, " ")
				output_paths:write(path[j].y, " ")
				output_paths:write(path[j].duration, "\n")
			end
		end
	end
end
output_paths:close()

local click_durations = {}
local is_click = false
local start_x, start_y
local current_duration = 0
local frame_count = 0
for i = 1, #frames do
	if frames[i].buttons[1] then
		if is_click then
			current_duration = current_duration + frames[i].duration
			frame_count = frame_count + 1
		else
			start_x = frames[i].position.x
			start_y = frames[i].position.y
			is_click = true
		end
	else
		if is_click then
			local d_x = start_x - frames[i].position.x
			local d_y = start_y - frames[i].position.y
			local distance = math.sqrt(d_x ^ 2 + d_y ^ 2)
			if distance < 4 then
				if current_duration == 0 then
					print(frame_count)
				end
				table.insert(click_durations, current_duration)
			end

			current_duration = 0
			frame_count = 0
			is_click = false
		end
	end
end
table.sort(click_durations)

print("click", "min", "max", "median")
local min = click_durations[1]
local max = click_durations[#click_durations]
local median = click_durations[math.floor(#click_durations / 2)]
print("", min, max, median)