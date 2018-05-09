-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Success = B.Node("Success")

function Success:update(mashina, state, executor)
	local child = self.tree:children(self.node)
	if child then
		local r = executor:update(child)
		if r == B.Status.Working then
			return B.Status.Working
		else
			return B.Status.Success
		end
	end

	return B.Status.Success
end

return Success
