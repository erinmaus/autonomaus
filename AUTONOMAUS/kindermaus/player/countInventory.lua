-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local CountInventory = B.Node("Player.CountInventory")
CountInventory.ITEM = B.Reference()
CountInventory.RESULT = B.Reference()
CountInventory.UPDATE = B.Local()

function CountInventory:update(mashina, state, executor)
	return state[self.UPDATE]:execute(executor)
end

function CountInventory:deactivated(mashian, state)
	state[self.UPDATE] = nil
end

function CountInventory:activated(mashina, state, executor)
	local update = B.TreeBuilder.Root() {
		K.Sequence {
			K.GUI.InventoryPanel.Open,
			K.GUI.InventoryPanel.GetCountItem {
				item = state[self.ITEM],
				[self.RESULT] = B.Output.RESULT
			}
		}
	}

	state[self.UPDATE] = B.TreeBuilder.materialize(update)
end

return CountInventory
