-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Try = B.Node("Try")
Try.INDEX = B.Local()

function Try:update(mashina, state, executor)
	local index = state[self.INDEX] or 1
	local children = { self.tree:children(self.node) }
	for i = 1, #children do
		if i >= index then
			local r = executor:update(children[i])
			if r == B.Status.Working then
				return B.Status.Working
			elseif r == B.Status.Success then
				executor:drop()
				return B.Status.Success
			else
				index = index + 1
				state[self.INDEX] = index
			end
		else
			executor:visit(children[i])
		end
	end

	executor:drop()
	return B.Status.Failure
end

return Try
