-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Execute = B.Node("Execute")
Execute.TREE = B.Reference()

function Execute:update(mashina, state, executor)
	local t = state[self.TREE]
	if t then
		return t:execute(executor)
	else
		return B.Status.Failure
	end
end

return Execute
