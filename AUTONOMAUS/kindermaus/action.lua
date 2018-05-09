-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Action = B.Node("Action")
Action.INDEX = B.Local()

function Action:update(mashina, state, executor)
	-- TODO: Not yet implemented. Should notify B.Mashina cortex of failure,
	-- then try again.
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
				return B.Status.Working
			else
				index = index + 1
				state[self.INDEX] = index
			end
		else
			executor:visit(children[i])
		end
	end

	-- TODO: Poke B.Mashina
	state[self.INDEX] = nil
	executor:drop()
	return B.Status.Success
end

function Action:deactivated(mashina, state)
	state[self.INDEX] = nil
end

return Action
