-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Wait = B.Node("Wait")
Wait.TIMEOUT = B.Reference()
Wait.TIME = B.Local()

function Wait:update(mashina, state, executor)
	local child = self.tree:children(self.node)
	if child then
		local r = executor:update(child)
		if r == B.Status.Success then
			executor:drop()
			return B.Status.Success
		end
	end

	local time = state[self.TIME] or 0
	if A.System.time() > time + (state[self.TIMEOUT] or 0) then
		if child then
			executor:drop()
			return B.Status.Failure
		else
			return B.Status.Success
		end
	end

	return B.Status.Working
end

function Wait:deactivated(mashina, state)
	state[self.TIME] = nil
end

function Wait:activated(mashina, state)
	state[self.TIME] = A.System.time()
end

return Wait
