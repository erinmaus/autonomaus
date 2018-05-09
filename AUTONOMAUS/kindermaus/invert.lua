-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Invert = B.Node("Invert")

function Invert:update(mashina, state, executor)
	local child = self.tree:children(self.node)
	if child then
		local r = executor:update(child)
		if r == B.Status.Success then
			return B.Status.Failure
		elseif r == B.Status.Failure then
			return B.Status.Success
		else
			return B.Status.Working
		end
	end

	return B.Status.Success
end

return Invert
