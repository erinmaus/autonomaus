-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Add = B.Node("Add")
Add.LEFT = B.Reference()
Add.RIGHT = B.Reference()
Add.RESULT = B.Reference()

function Add:update(mashina, state, executor)
	local a = state[self.LEFT] or 0
	local b = state[self.RIGHT] or 0
	state[self.RESULT] = a + b

	return B.Status.Success
end

return Add
