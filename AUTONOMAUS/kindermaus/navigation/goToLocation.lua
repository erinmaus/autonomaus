-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local GoToLocation = B.Node("Navigation.GoToLocation")
GoToLocation.LOCATION = B.Reference()
GoToLocation.DISABLE_MINIMAP = B.Reference()
GoToLocation.RESULT = B.Local()
GoToLocation.EXECUTED = B.Local()

function GoToLocation:update(mashina, state, executor)
	if state[self.EXECUTED] then
		local view = A.Cortex.get("Game::View")
		local from = A.Cortex.pull(view, "target_tile")
		if from then
			from = A.Game.Atlas.locate(from)
		end

		if not from then
			return B.Status.Failure
		end

		local to = A.Game.Atlas.find(state[self.LOCATION])[1]
		if not to then
			return B.Status.Failure
		end

		if from.id == to.id then
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
			local plan = A.Cortex.Result.message(result)

			A.Cortex.poke(mashina, "execute", plan)
			state[self.EXECUTED] = true
		elseif A.Cortex.Result.failed(result) then
			return B.Status.Failure
		end

		return B.Status.Working
	end
end

function GoToLocation:deactivated(mashina, state)
	state[self.EXECUTED] = nil
	state[self.RESULT] = nil

	if state[self.DISABLE_MINIMAP] then
		local walker = A.Cortex.get("Game::Walker")
		A.Cortex.poke(walker, "enable_minimap")
	end
end

function GoToLocation:activated(mashina, state, executor)
	if not state[self.LOCATION] then
		A.Log.warn("(Navigation.GoToLocation) No destination location specified.")
		return
	end

	local mashina = A.Cortex.get("B::Mashina")
	local view = A.Cortex.get("Game::View")
	local from = A.Cortex.pull(view, "target_tile")
	if from then
		from = A.Game.Atlas.locate(from)
	end

	if not from then
		A.Log.warn("(Navigation.GoToLocation) Couldn't find current location. Defaulting to Gielinor.")
		from = A.Game.Atlas.find("Gielinor")[1]

		if not from then
			A.Log.warn("(Navigation.GoToLocation) Couldn't get Gielinor location. Database bad!")
			return
		end
	end

	local to = A.Game.Atlas.find(state[self.LOCATION])[1]
	if not to then
		A.Log.warn("(Navigation.GoToLocation) Couldn't find destination location '%s'. Woops!", state[self.LOCATION])
	end

	if from.id == to.id then
		state[self.EXECUTED] = true
	else
		local m = {
			from = from.id,
			to = to.id,
			wizard = "travel_without_bank"
		}
		state[self.RESULT] = A.Cortex.poke(mashina, "visit", m)
	end

	if state[self.DISABLE_MINIMAP] then
		local walker = A.Cortex.get("Game::Walker")
		A.Cortex.poke(walker, "disable_minimap")
	end
end

return GoToLocation
