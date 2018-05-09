-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local EquipItem = B.Node("Player.EquipItem")
EquipItem.ITEM = B.Reference()
EquipItem.UPDATE = B.Local()

function EquipItem:update(mashina, state, executor)
	return state[self.UPDATE]:execute(executor)
end

function EquipItem:deactivated(mashian, state)
	state[self.UPDATE] = nil
end

function EquipItem:activated(mashina, state, executor)
	local action_bar = A.Cortex.get("GUI::ActionBar")
	local m = { type = A.Cortex.GUI.ActionBar.type_item, value = state[self.ITEM] }
	local result = A.Cortex.pull(action_bar, "find", m)

	local update
	if result then
		update = B.TreeBuilder.Root() {
			K.Sequence {
				K.GUI.ActionBar.Activate {
					type = A.Cortex.GUI.ActionBar.type_item,
					value = state[self.ITEM]
				}
			}
		}
	else
		update = B.TreeBuilder.Root() {
			K.Sequence {
				K.GUI.InventoryPanel.Open,
				K.GUI.InventoryPanel.PokeItem {
					item = state[self.ITEM],
					action = "%(Wield|Wear).*"
				}
			}
		}
	end

	state[self.UPDATE] = B.TreeBuilder.materialize(update)
end

return EquipItem
