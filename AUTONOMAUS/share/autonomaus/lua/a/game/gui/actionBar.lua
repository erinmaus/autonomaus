-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local ActionBar = {}
ActionBar.action_bar_none = -1
ActionBar.action_bar_primary = 0
ActionBar.action_bar_auxiliary_1 = 1
ActionBar.action_bar_auxiliary_2 = 2
ActionBar.action_bar_auxiliary_3 = 3
ActionBar.action_bar_auxiliary_4 = 4

ActionBar.type_none = 0
ActionBar.type_ability = 1
ActionBar.type_item = 2
ActionBar.type_spell = 3
ActionBar.type_prayer = 4
ActionBar.type_unknown = 5

function ActionBar.find_best_ability(abilities, cooldown)
	cooldown = cooldown or 1

	local action_bar = A.Cortex.get("GUI::ActionBar")
	for i = 1, #abilities do
		local slot
		do
			local m = {
				type = ActionBar.type_ability,
				value = abilities[i]
			}

			slot = A.Cortex.pull(action_bar, "find", m)
		end

		if slot then
			local m = {}
			if type(slot.action_bar) == "table" then
				m.action_bar = slot.action_bar[1]
				m.slot = slot.slot[1]
			else
				m.action_bar = slot.action_bar
				m.slot = slot.slot
			end

			slot = A.Cortex.pull(action_bar, "slot", m)
		end

		if slot and slot.enabled then
			local current_cooldown = A.Cortex.pull(action_bar, "cooldown", { ability = abilities[i] })
			if current_cooldown and current_cooldown.result <= cooldown then
				return abilities[i]
			end
		end
	end

	return nil
end

return ActionBar
