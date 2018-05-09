-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

return function(cortex, poke, args, aliases)
	local Cortex = B.Action("poke::A.Cortex." .. cortex .. "." .. poke)
	Cortex.RESULT = B.Local()

	function Cortex:update(mashina, state, executor)
		local result = state[self.RESULT]

		if result then
			if A.Cortex.Result.pending(result) then
				return B.Status.Working
			elseif A.Cortex.Result.failed(result) then
				return B.Status.Failure
			elseif A.Cortex.Result.succeeded(result) then
				return B.Status.Success
			end
		end

		return B.Status.Failure
	end

	function Cortex:activated(mashina, state)
		local cortex = A.Cortex.get(cortex)

		local a = {}
		for key, value in pairs(args) do
			a[key] = state[aliases[value] or value] or value
		end

		state[self.RESULT] = A.Cortex.poke(cortex, poke, a)
	end

	function Cortex:deactivated(mashina, state)
		local result = state[self.RESULT]
		if result then
			A.Cortex.Result.cancel(result)
		end
	end

	return Cortex
end
