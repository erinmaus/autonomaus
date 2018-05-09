-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local UsePrayer = B.Node("Player.UsePrayer")
UsePrayer.PRAYER = B.Reference()
UsePrayer.RESULT = B.Local()
UsePrayer.COOLDOWN = B.Local()

function UsePrayer:update(mashina, state, executor)
	local result = state[self.RESULT]
	if result == true then
		local cooldown = state[self.COOLDOWN]
		if A.System.time() > cooldown then
			local buff_bar = A.Cortex.get("GUI::BuffBar")
			local r = A.Cortex.pull(buff_bar, "has_buff", state[self.PRAYER])

			if not r or not r.result then
				self:deactivate(executor)
				return B.Status.Working
			end
		end

		return B.Status.Success
	elseif A.Cortex.Result.pending(result) then
		return B.Status.Working
	elseif A.Cortex.Result.succeeded(result) then
		state[self.RESULT] = true
		state[self.COOLDOWN] = A.System.time() + 0.6
		return B.Status.Success
	else
		return B.Status.Failure
	end
end

function UsePrayer:deactivated(mashian, state)
	state[self.RESULT] = nil
end

function UsePrayer:activated(mashina, state, executor)
	local buff_bar = A.Cortex.get("GUI::BuffBar")
	local r = A.Cortex.pull(buff_bar, "has_buff", state[self.PRAYER])
	if r and r.result then
		state[self.RESULT] = true
	else
		local action_bar = A.Cortex.get("GUI::ActionBar")
		state[self.RESULT] = A.Cortex.poke(action_bar, "activate", {
			type = A.Cortex.GUI.ActionBar.type_prayer,
			value = state[self.PRAYER]
		})
	end
end

return UsePrayer
