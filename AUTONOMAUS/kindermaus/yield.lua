-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local Yield = B.Node("Yield")

function Yield:update(mashina, state, executor)
	-- TODO: Not yet implemented. Should be a shortcut for K.B.Mashina.Yield.
	return B.Status.Success
end

return Yield
