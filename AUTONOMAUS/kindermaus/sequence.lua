-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Sequence = B.Node("Sequence")

function Sequence:update(mashina, state, executor)
	local children = { self.tree:children(self.node) }
	for i = 1, #children do
		local r = executor:update(children[i])
		if r == B.Status.Working then
			return B.Status.Working
		elseif r == B.Status.Failure then
			executor:drop()
			return B.Status.Failure
		end
	end

	executor:drop()
	return B.Status.Success
end

return Sequence
