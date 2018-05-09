-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local WaitForIdle = B.Node("Game.World.WaitForIdle")
WaitForIdle.TIMEOUT = B.Reference()
WaitForIdle.DEFAULT_TIMEOUT = 0.4

function WaitForIdle:update(mashina, state, executor)
	local view = A.Cortex.get("Game::View")
	local idle = A.Cortex.pull(view, "target_idle_time", {})
	local animations = A.Cortex.pull(view, "target_animations", {})
	local timeout = state[self.TIMEOUT] or self.DEFAULT_TIMEOUT

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

	if not is_idle then
		A.Cortex.poke(view, "reset_target_idle_time", {})
	elseif idle.result > timeout then
		return B.Status.Success
	end

	return B.Status.Working
end

function WaitForIdle:activated(mashina, state, executor)
	local view = A.Cortex.get("Game::View")
	A.Cortex.poke(view, "reset_target_idle_time", {})
end

return WaitForIdle
