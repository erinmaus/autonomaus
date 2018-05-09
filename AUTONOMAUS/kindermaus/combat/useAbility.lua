-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local UseAbility = B.Node("Combat.UseAbility")
UseAbility.ABILITY = B.Reference()
UseAbility.QUEUE = B.Reference()
UseAbility.RESULT = B.Local()
UseAbility.COOLDOWN = B.Local()
UseAbility.TIME = B.Local()

function UseAbility:update(mashina, state, executor)
	local result = state[self.RESULT]
	if result == nil then
		return B.Status.Failure
	else
		if A.Cortex.Result.failed(result) then
			return B.Status.Failure
		elseif not A.Cortex.Result.pending(result) then
			local action_bar = A.Cortex.get("GUI::ActionBar")
			local cooldown = A.Cortex.pull(action_bar, "cooldown", { ability = state[self.ABILITY] })
			if cooldown and cooldown.result then
				if cooldown.result > state[self.COOLDOWN] then
					return B.Status.Success
				else
					local t = state[self.TIME] + state[self.COOLDOWN] + 0.9
					if A.System.time() > t then
						return B.Status.Failure
					end
				end
			end
		end
	end

	return B.Status.Working
end

function UseAbility:deactivated(mashina, state, executor)
	local result = state[self.RESULT]
	if result then
		A.Cortex.Result.cancel(result)
		state[self.RESULT] = nil
		state[self.COOLDOWN] = nil
	end
end

function UseAbility:activated(mashina, state, executor)
	local queue = state[self.QUEUE]
	if queue == nil then
		queue = true
	end

	local action_bar = A.Cortex.get("GUI::ActionBar")
	local cooldown = A.Cortex.pull(action_bar, "cooldown", { ability = state[self.ABILITY] })
	if cooldown and cooldown.result then
		if queue or cooldown.result <= 2 then
			state[self.TIME] = A.System.time()
			state[self.COOLDOWN] = cooldown.result
			local m = {
				type = A.Cortex.GUI.ActionBar.type_ability,
				value = state[self.ABILITY]
			}
			state[self.RESULT] = A.Cortex.poke(action_bar, "activate", m)
		end
	end
end

return UseAbility
