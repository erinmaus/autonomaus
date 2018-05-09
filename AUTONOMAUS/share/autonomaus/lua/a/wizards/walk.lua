-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local W = A.Game.Wizard("travel_walk")

function W:cost(action, quantity)
	return (action.cost or 1) * quantity
end

function W:distance(from, to, action)
	return (action.cost or 1)
end

function W:neighbors(resource)
	local actions = resource:actions()

	local function filter_action(action)
		return action.type == "travel"
		       and action.name ~= "teleport"
	end

	local function filter_requirement(requirement)
		return not requirement.is_input and
		       not requirement.is_output and
		       requirement.resource.type == "location"
	end

	return A.Game.Luggage.neighbors(actions, filter_action, filter_requirement)
end
