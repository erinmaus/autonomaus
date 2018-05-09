-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local IsResourceVisible = B.Node("Game.World.IsResourceVisible")
IsResourceVisible.TILE = B.Reference()
IsResourceVisible.NAME = B.Reference()

function IsResourceVisible:update(mashina, state, executor)
	local world = A.Cortex.get("Game::World")
	local objects = A.Cortex.pull(world, "nearby_objects", { name = state[self.NAME] })

	if not objects then
		return B.Status.Failure
	end

	local current = state[self.TILE]
	for i = 1, #objects.x do
		local x = objects.x[i]
		local y = objects.y[i]
		local layer = objects.layer[i]

		if x == current.x and y == current.y and layer == current.layer then
			return B.Status.Success
		end
	end

	return B.Status.Failure
end

return IsResourceVisible
