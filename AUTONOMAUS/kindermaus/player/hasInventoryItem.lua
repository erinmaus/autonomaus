-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local HasInventoryItem = B.Node("Player.HasInventoryItem")
HasInventoryItem.ITEM = B.Reference()
HasInventoryItem.COUNT = B.Local()
HasInventoryItem.UPDATE = B.Local()

function HasInventoryItem:update(mashina, state, executor)
	local r = state[self.UPDATE]:execute(executor)
	if r == B.Status.Success then
		if state[self.COUNT] > 0 then
			return B.Status.Success
		else
			return B.Status.Failrue
		end
	end

	return r
end

function HasInventoryItem:deactivated(mashian, state)
	state[self.UPDATE] = nil
end

function HasInventoryItem:activated(mashina, state, executor)
	local update = B.TreeBuilder.Root() {
		K.Sequence {
			K.GUI.InventoryPanel.Open,
			K.GUI.InventoryPanel.GetCountItem {
				item = state[self.ITEM],
				[self.COUNT] = B.Output.RESULT
			}
		}
	}

	state[self.UPDATE] = B.TreeBuilder.materialize(update)
end

return HasInventoryItem
