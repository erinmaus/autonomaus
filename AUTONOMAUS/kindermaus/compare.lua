-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Compare = B.Node("Compare")
Compare.LEFT = B.Reference()
Compare.RIGHT = B.Reference()
Compare.CONDITION = B.Reference()

function Compare:update(mashina, state, executor)
	local a = state[self.LEFT]
	local b = state[self.RIGHT]
	local c = state[self.CONDITION] or function() return false end
	
	if c(a, b) then
		return B.Status.Success
	else
		return B.Status.Failure
	end
end

return Compare
