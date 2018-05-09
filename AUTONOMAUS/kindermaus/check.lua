-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Check = B.Node("Check")
Check.CONDITION = B.Reference()

function Check:update(mashina, state)
	local a = state[self.CONDITION]

	if a then
		return B.Status.Success
	else
		return B.Status.Failure
	end
end

return Check
