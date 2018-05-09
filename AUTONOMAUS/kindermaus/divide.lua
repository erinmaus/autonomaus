-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Divide = B.Node("Divide")
Divide.LEFT = B.Reference()
Divide.RIGHT = B.Reference()
Divide.RESULT = B.Reference()

function Divide:update(mashina, state, executor)
	local a = state[self.LEFT] or 0
	local b = state[self.RIGHT] or 0
	state[self.RESULT] = a / b

	return B.Status.Success
end

return Divide
