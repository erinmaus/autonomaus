-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local IsInventoryEmpty = B.Node("Player.IsInventoryEmpty")
IsInventoryEmpty.QUANTITY = B.Local()
IsInventoryEmpty.VISIBLE = B.Local()
IsInventoryEmpty.UPDATE = B.Local()

function IsInventoryEmpty:update(mashina, state, executor)
	local status = state[self.UPDATE]:execute(executor)
	if status == B.Status.Success then
		if state[self.QUANTITY] < 1 then
			return B.Status.Success
		else
			return B.Status.Failure
		end
	end

	return status
end

function IsInventoryEmpty:activated(mashina, state, executor)
	local update = B.TreeBuilder.Root() {
		K.Sequence {
			K.Selector {
				K.Sequence {
					K.GUI.InventoryPanel.GetIsVisible {
						[self.VISIBLE] = B.Output.RESULT,
					},

					K.Check { condition = self.VISIBLE }
				},

				K.GUI.InventoryPanel.Open
			},

			K.GUI.InventoryPanel.GetUsedSpace {
				[self.QUANTITY] = B.Output.RESULT
			}
		}
	}

	state[self.UPDATE] = B.TreeBuilder.materialize(update)
end

return IsInventoryEmpty
