-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Equal = B.Node("Equal")
Equal.A = B.Reference()
Equal.B = B.Reference()

function Equal:update(mashina, state)
	local a = state[self.A]
	local b = state[self.B]

	if a == b then
		return B.Status.Success
	else
		return B.Status.Failure
	end
end

return Equal
