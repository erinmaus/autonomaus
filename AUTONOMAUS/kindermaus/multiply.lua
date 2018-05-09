-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Multiply = B.Node("Multiply")
Multiply.LEFT = B.Reference()
Multiply.RIGHT = B.Reference()
Multiply.RESULT = B.Reference()

function Multiply:update(mashina, state, executor)
	local a = state[self.LEFT] or 0
	local b = state[self.RIGHT] or 0
	state[self.RESULT] = a * b

	return B.Status.Success
end

return Multiply
