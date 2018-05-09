-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local W = A.Game.Wizard("travel_without_bank")

function W:cost(action, quantity)
	return (action.cost or 1) * quantity
end

function W:distance(from, to, action)
	return A.Game.Luggage.distance(from, to, action)
end

function W:neighbors(resource)
	local actions = resource:actions()
	local neighbors = {}

	local function filter_action(action)
		if action.type == "travel" then
			local resource = action.resource
			if A.Game.Luggage.lock(resource) then
				return A.Game.Luggage.unlocked(resource)
			else
				return true
			end
		end
	end

	local function filter_requirement(requirement)
		return not requirement.is_output and
		       not requirement.is_input and
		       requirement.resource.type == "location"
	end

	A.Game.Luggage.neighbors(
		resource:actions(),
		filter_action,
		filter_requirement,
		neighbors)
	A.Game.Luggage.neighbors(
		A.Game.Actions.find("travel", "teleport"),
		filter_action,
		filter_requirement,
		neighbors)

	return neighbors
end

function W:has(resource, count)
	return A.Game.Luggage.has(resource, count, { include_bank = false, include_inventory = true })
end

function W:count(resource)
	return A.Game.Luggage.count(resource, { include_bank = false, include_inventory = true })
end
