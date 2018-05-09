-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Selector = B.Node("Selector")

function Selector:update(mashina, state, executor)
	local children = { self.tree:children(self.node) }
	for i = 1, #children do
		local r = executor:update(children[i])
		if r == B.Status.Working then
			return B.Status.Working
		elseif r == B.Status.Success then
			executor:drop()
			return B.Status.Success
		end
	end

	executor:drop()
	return B.Status.Failure
end

return Selector
