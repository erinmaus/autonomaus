-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Repeat = B.Node("Repeat")

function Repeat:update(mashina, state, executor)
	local children = { self.tree:children(self.node) }
	for i = 1, #children do
		local r = executor:update(children[i])
		if r == B.Status.Working then
			return B.Status.Working
		elseif r == B.Status.Failure then
			executor:drop()
			return B.Status.Success
		end
	end

	return B.Status.Working
end

return Repeat
