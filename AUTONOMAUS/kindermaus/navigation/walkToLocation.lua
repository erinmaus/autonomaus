-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local WalkToLocation = B.Node("Navigation.WalkToLocation")
WalkToLocation.LOCATION = B.Reference()
WalkToLocation.DISABLE_MINIMAP = B.Reference()
WalkToLocation.RESULT = B.Local()
WalkToLocation.EXECUTED = B.Local()

function WalkToLocation:update(mashina, state, executor)
	if state[self.EXECUTED] then
		local view = A.Cortex.get("Game::View")
		local current_tile = A.Cortex.pull(view, "target_tile")
		local from
		if current_tile then
			from = A.Game.Atlas.locate(current_tile)
		end

		if not from then
			return B.Status.Failure
		end

		local to = A.Game.Atlas.find(state[self.LOCATION])[1]
		if not to then
			return B.Status.Failure
		end

		if from.id == to.id or to:inside(current_tile) then
			return B.Status.Success
		else
			return B.Status.Failure
		end
	else
		local result = state[self.RESULT]

		if not result then
			return B.Status.Failure
		end

		if A.Cortex.Result.succeeded(result) then
			local mashina = A.Cortex.get("B::Mashina")
			local message = A.Cortex.Result.message(result)

			local plan = { count = { 1 } }
			if type(message.action_id) == 'table' then
				plan.action_id = { message.action_id[#message.action_id] }
			else
				plan.action_id = { message.action_id }
			end

			A.Cortex.poke(mashina, "execute", plan)
			state[self.EXECUTED] = true
		elseif A.Cortex.Result.failed(result) then
			return B.Status.Failure
		end

		return B.Status.Working
	end
end

function WalkToLocation:deactivated(mashina, state)
	state[self.EXECUTED] = nil
	state[self.RESULT] = nil

	if state[self.DISABLE_MINIMAP] then
		local walker = A.Cortex.get("Game::Walker")
		A.Cortex.poke(walker, "enable_minimap")
	end
end

function WalkToLocation:activated(mashina, state, executor)
	if not state[self.LOCATION] then
		A.Log.warn("(Navigation.WalkToLocation) No destination location specified.")
		return
	end

	local mashina = A.Cortex.get("B::Mashina")
	local view = A.Cortex.get("Game::View")
	local current_tile = A.Cortex.pull(view, "target_tile")
	local from
	if current_tile then
		from = A.Game.Atlas.locate(current_tile)
	end

	if not from then
		A.Log.warn("(Navigation.WalkToLocation) Couldn't find current location. Defaulting to Gielinor.")
		from = A.Game.Atlas.find("Gielinor")[1]

		if not from then
			A.Log.warn("(Navigation.WalkToLocation) Couldn't get Gielinor location. Database bad!")
			return
		end
	end

	local to = A.Game.Atlas.find(state[self.LOCATION])[1]
	if not to then
		A.Log.warn("(Navigation.WalkToLocation) Couldn't find destination location '%s'. Woops!", state[self.LOCATION])
	end

	if from.id == to.id and from.anchor.layer == current_tile.layer then
		state[self.EXECUTED] = true
	else
		local m = {
			from = from.id,
			to = to.id,
			wizard = "travel_walk"
		}
		state[self.RESULT] = A.Cortex.poke(mashina, "visit", m)
	end

	if state[self.DISABLE_MINIMAP] then
		local walker = A.Cortex.get("Game::Walker")
		A.Cortex.poke(walker, "disable_minimap")
	end
end

return WalkToLocation
