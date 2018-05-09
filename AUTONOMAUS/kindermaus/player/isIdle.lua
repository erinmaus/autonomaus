-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local IsIdle = B.Node("Player.IsIdle")

function IsIdle:update(mashina, state, executor)
	local view = A.Cortex.get("Game::View")
	local idle = A.Cortex.pull(view, "target_idle_time", {})
	local animations = A.Cortex.pull(view, "target_animations", {})

	local is_idle = false
	if animations then
		if type(animations.result) == 'string' and animations.result == "idle" then
			is_idle = true
		else
			for i = 1, #animations.result do
				if animations.result[i] == "idle" then
					is_idle = true
					break
				end
			end
		end
	end

	if is_idle and idle.result > 0 then
		return B.Status.Success
	else
		return B.Status.Failure
	end
end

return IsIdle
