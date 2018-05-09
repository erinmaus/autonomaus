-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local WithinRadius = B.Node("Navigation.WithinRadius")
WithinRadius.TILE = B.Reference()
WithinRadius.RADIUS = B.Reference()

function WithinRadius:update(mashina, state, executor)
	local tile = state[self.TILE]
	if not tile then
		return B.Status.Failure
	end

	local view = A.Cortex.get("Game::View")
	local current = A.Cortex.pull(view, "target_tile", {})
	if not current then
		return B.Status.Failure
	end

	if tile.layer ~= current.layer then
		return B.Status.Failure
	end

	local dx = current.x - tile.x
	local dy = current.y - tile.y

	local distance = dx ^ 2 + dy ^ 2
	local radius = state[self.RADIUS] or 1

	if distance < radius * radius then
		return B.Status.Success
	else
		return B.Status.Failure
	end
end

return WithinRadius
