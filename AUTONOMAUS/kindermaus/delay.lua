-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Delay = B.Node("Delay")
Delay.TIMEOUT = B.Reference()
Delay.TIME = B.Local()
Delay.DONE = B.Local()

function Delay:update(mashina, state, executor)
	local child = self.tree:children(self.node)
	if child and not state[self.DONE] then
		local r = executor:update(child)
		if r == B.Status.Success then
			state[self.DONE] = true
		elseif r == B.Status.Failure then
			return B.Status.Failure
		end
	end

	local time = state[self.TIME] or 0
	if A.System.time() > time + (state[self.TIMEOUT] or 0) then
		if child then
			executor:drop()
		end

		if state[self.DONE] then
			return B.Status.Success
		else
			return B.Status.Failure
		end
	end

	return B.Status.Working
end

function Delay:deactivated(mashina, state)
	state[self.TIME] = nil
	state[self.DONE] = nil
end

function Delay:activated(mashina, state)
	state[self.TIME] = A.System.time()
	state[self.DONE] = false
end

return Delay
