-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Luggage = {}

function Luggage.distance(from, to, action)
	local offset = 0
	if action.type == "travel" and action.name == "walk" then
		local view = A.Cortex.get("Game::View")
		local current_tile = A.Cortex.pull(view, "target_tile")
		if current_tile then
			local f = A.Game.Atlas.locate(current_tile)
			if f and f.id == from.id then
				local dx = current_tile.x - to.anchor.x
				local dy = current_tile.y - to.anchor.y

				return math.sqrt(dx ^ 2 + dy ^ 2)
			end
		end

		return (action.cost or 1)
	end

	return (action.cost or 1)
end

function Luggage.neighbors(actions, filter_action, filter_requirement, neighbors, unlocked)
	if unlocked == nil then
		unlocked = true
	end

	actions = actions or {}
	filter_action = filter_action or function() return true end
	filter_requirement = filter_requirement or function() return true end
	neighbors = neighbors or {}

	for _, action in ipairs(actions) do
		if action.is_getter and filter_action(action) then
			local requirements = action:requirements()

			local satisfied = true
			if unlocked then
				local l, u = Luggage.unlocked(action.resource)
				if l and not u then
					satisfied = false
				else
					for _, requirement in ipairs(requirements) do
						if not requirement.is_output and
						   not Luggage.has(requirement.resource, requirement.count, { include_bank = true })
						then
							satisfied = false
							break
						end
					end
				end
			end

			if satisfied then
				for _, requirement in ipairs(requirements) do
					if filter_requirement(requirement) then
						table.insert(neighbors, { action, requirement.resource })
					end
				end
			end
		end
	end

	return neighbors
end

function Luggage.has(resource, count, options)
	count = count or 1

	return Luggage.count(resource, options) >= count
end

-- options for items:
--  * search_bank (bool): whether to search bank or not (defaults false)
--  * search_inventory (bool): whether to search inventory or not (defaults true)
--  * search_familiar (bool): whether to search familiar or not (defaults false)
--  * include_noted (bool): whether to include noted items or not (defaults false)
function Luggage.count(resource, options)
	local o = table.clone(options or {})
	if o.search_inventory == nil then
		o.search_inventory = true
	end

	local count = 0
	if resource.type == "item" then
		if options.search_bank == true then
			local bank = A.Cortex.get("GUI::BankWindow")
			count = count + A.Cortex.pull("count_item", { item = resource.name }).result
		end

		if options.search_inventory == true then
			local inventory = A.Cortex.get("GUI::InventoryPanel")
			local m = { item = resource.name }

			-- TODO: match_noted, match_unnoted, match_variants
			m.flags = 1 + 4
			if m.include_noted then
				m.flags = m.flags + 2
			end

			count = count + A.Cortex.pull("count_item", m).result
		end

		if options.search_familiar == true then
			-- TODO
		end
	end

	return count
end

local Keys = {}
function Luggage.key(name, type, callback)
	if not Keys[type] then
		Keys[type] = {}
	end

	Keys[type][name] = callback
end

function Luggage.lock(resource)
	if Keys[resource.type] and Keys[resource.name] then
		return true
	end

	return false
end

function Luggage.unlocked(resource)
	local locks = Keys[resource.type]
	if locks then
		lock = locks[resource.name]
		if lock then
			return true, lock()
		end
	end

	return false, Luggage.has(resource)
end

return Luggage
