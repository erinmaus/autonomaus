-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Print = B.Node("Print")
Print.MESSAGE = B.Reference()

function Print:update(mashina, state)
	local a = state[self.MESSAGE]

	if a then
		print(a)
	else
		print("no message")
	end

	return B.Status.Success
end

return Print
