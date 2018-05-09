-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Step = B.Node("Step")
Step.INDEX = B.Local()

function Step:update(mashina, state, executor)
	local index = state[self.INDEX] or 1
	local children = { self.tree:children(self.node) }
	for i = 1, #children do
		if i >= index then
			local r = executor:update(children[i])
			if r == B.Status.Working then
				return B.Status.Working
			elseif r == B.Status.Failure then
				state[self.INDEX] = nil
				executor:drop()
				return B.Status.Failure
			else
				index = index + 1
				state[self.INDEX] = index
			end
		else
			executor:visit(children[i])
		end
	end

	state[self.INDEX] = nil
	executor:drop()
	return B.Status.Success
end

function Step:deactivated(mashina, state)
	state[self.INDEX] = nil
end

function Step:activated(mashian, state)
	state[self.INDEX] = 1
end

return Step
