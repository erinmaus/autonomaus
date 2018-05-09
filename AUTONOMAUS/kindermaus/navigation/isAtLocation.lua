-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local IsAtLocation = B.Node("Navigation.IsAtLocation")
IsAtLocation.LOCATION = B.Reference()

function IsAtLocation:update(mashina, state, executor)
	local location = state[self.LOCATION]
	if not location then
		return B.Status.Failure
	end

	location = A.Game.Atlas.find(location)[1]
	if not location then
		return B.Status.Failure
	end

	local view = A.Cortex.get("Game::View")
	local current = A.Cortex.pull(view, "target_tile", {})
	if not current then
		return B.Status.Failure
	end

	current = A.Game.Atlas.locate(current)
	if not current then
		return B.Status.Failure
	end

	if location.id == current.id then
		return B.Status.Success
	else
		return B.Status.Failure
	end
end

return IsAtLocation
