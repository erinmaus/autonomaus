-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Subtract = B.Node("Subtract")
Subtract.LEFT = B.Reference()
Subtract.RIGHT = B.Reference()
Subtract.RESULT = B.Reference()

function Subtract:update(mashina, state, executor)
	local a = state[self.LEFT] or 0
	local b = state[self.RIGHT] or 0
	state[self.RESULT] = a - b

	return B.Status.Success
end

return Subtract
