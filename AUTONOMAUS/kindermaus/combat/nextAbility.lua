-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local NextAbility = B.Node("Combat.NextAbility")
NextAbility.ABILITIES = B.Reference()
NextAbility.COOLDOWN = B.Reference()
NextAbility.RESULT = B.Reference()

function NextAbility:update(mashina, state, executor)
	local abilities = state[self.ABILITIES] or {}
	local result = A.Cortex.GUI.ActionBar.find_best_ability(abilities)
	if result == nil then
		print("No good ability.", #abilities)
		return B.Status.Failure
	end

	state[self.RESULT] = result
	return B.Status.Success
end

return NextAbility
