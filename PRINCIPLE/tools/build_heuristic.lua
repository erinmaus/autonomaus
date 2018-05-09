local csv = require("csv")

local args = { ... }
if #args < 3 then
	print("build_heuristic <candidates.csv> <tile_properties.csv> <tile_results.csv> [property 1, [property N]]")
	os.exit(1)
end

function printfln(format, ...)
	print(string.format(format, ...))
end

local candidates = { tags = {} }
do
	local file = csv.open(args[1], { header = true })
	for candidate in file:lines() do
		for key, value in pairs(candidate) do
			candidate[key] = tonumber(value) or value
		end
		table.insert(candidates, candidate)
		candidates.tags[candidate.candidate_tag] = candidate
	end
end
do
	local file = csv.open(args[2], { header = true })
	for tile in file:lines() do
		for key, value in pairs(tile) do
			tile[key] = tonumber(value) or value
		end

		local tag = tile.candidate_tag
		local candidate = candidates.tags[tag]
		local tiles = candidate.tiles or {}
		table.insert(tiles, tile)
		candidate.tiles = tiles
	end
end

function find_candidate_by_tag(tag)
	return candidates.tags[tag]
end

function find_candidates_by_tile(tile_x, tile_y, tile_layer)
	local results = {}
	for _, candidate in ipairs(candidates) do
		local tiles = candidate.tiles or {}
		for _, tile in ipairs(tiles) do
			if tile.tile_x == tile_x and tile.tile_y == tile_y and tile.tile_layer == tile_layer then
				table.insert(results, candidate)
				results[candidate] = tile
			end
		end
	end
	return results
end

local results = {}
do
	local file = csv.open(args[3], { header = true })
	for result in file:lines() do
		for key, value in pairs(result) do
			result[key] = tonumber(value) or value
		end

		if result.passable == 'TRUE' then
			result.passable = true
			table.insert(results, result)
		elseif result.passable == 'FALSE' then
			result.passable = false
			table.insert(results, result)
		end
	end
end

local passable_tiles = { samples = 0 }
do
	for _, tile in pairs(results) do
		if not tile.passable then
			local candidates = find_candidates_by_tile(tile.tile_x, tile.tile_y, tile.tile_layer)
			local count = 0

			for i = 1, #candidates do
				local candidate = candidates[i]
				local tile_properties = candidates[candidate]

				local discard = false

				if tile_properties.num_token_intersections == 0 then
					discard = true
				end

				if candidate.surface_area_sum < 16000 then
					discard = true
				end

				do
					local l = tile_properties.left_triangles
					local r = tile_properties.right_triangles
					local t = tile_properties.top_triangles
					local b = tile_properties.bottom_triangles
					local n = tile_properties.num_triangles

					if l >= n or r >= n or t >= n or b >= n then
						discard = true
					end

					if ((l + t) > n and b == 0 and r == 0) or
					   ((l + b) > n and t == 0 and r == 0) or
					   ((r + t) > n and l == 0 and b == 0) or
					   ((r + b) > n and l == 0 and t == 0)
					then
						discard = true
					end
				end

				if not discard then
					count = count + 1
				end
			end

			local relative_intersections
			if large_triangles > 0 then
				relative_intersections = large_intersections / large_triangles
			else
				relative_intersections = 0
			end

			if count == 0 and relative_intersections < 0.05 then
				table.insert(passable_tiles, tile)
			end

			passable_tiles.samples = passable_tiles.samples + 1
		end
	end

	printfln(
		"%f match success (%d post-filtering, %d samples total)",
		#passable_tiles / passable_tiles.samples * 100,
		#passable_tiles, passable_tiles.samples)
end



local passable_properties = {}
local impassable_properties = {}
local SKIP =
{
	['candidate_tag'] = true,
	['tile_x'] = true,
	['tile_y'] = true,
	['tile_layer'] = true
}

for _, result in ipairs(results) do
	local candidates = find_candidates_by_tile(result.tile_x, result.tile_y, result.tile_layer)
	local properties
	if result.passable then
		properties = passable_properties
	else
		properties = impassable_properties
	end

	for i = 1, #candidates do
		local candidate = candidates[i]
		local tile = candidates[candidate]

		for property, value in pairs(candidate) do
			if not SKIP[property] and type(value) == 'string' then
				if not properties[property] then
					properties[property] = { min = tonumber(value), max = tonumber(value) }
				else
					properties[property].min = math.min(properties[property].min, tonumber(value))
					properties[property].max = math.max(properties[property].max, tonumber(value))
				end
			end
		end

		for property, value in pairs(tile) do
			if not SKIP[property] then
				property = "tile_" .. property
				if not properties[property] then
					properties[property] = { min = tonumber(value), max = tonumber(value) }
				else
					properties[property].min = math.min(properties[property].min, tonumber(value))
					properties[property].max = math.max(properties[property].max, tonumber(value))
				end
			end
		end
	end
end

function print_properties(properties)
	local keys = {}
	for property, value in pairs(properties) do
		table.insert(keys, property)
	end

	table.sort(keys)

	for i = 1, #keys do
		local property = keys[i]
		local value = properties[property]
		printfln("  %-30s min = %d, max = %d", property, value.min, value.max)
	end
end

printfln("passable tile properties:")
print_properties(passable_properties)

printfln("impassable tile properties:")
print_properties(impassable_properties)
