-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local UseNearbyBank = B.Node("Game.World.UseNearbyBank")
UseNearbyBank.BANK_TILE = B.Local()
UseNearbyBank.INDEX = B.Local()
UseNearbyBank.VISIBLE = B.Local()
UseNearbyBank.UPDATE = B.Local()
UseNearbyBank.CLOSE = B.Local()

function UseNearbyBank:update(mashina, state, executor)
	local status = state[self.UPDATE]:execute(executor)
	if status ~= B.Status.Success then
		return status
	end

	local children = { self.tree:children(self.node) }
	local index = state[self.INDEX] or 1
	for i = 1, #children do
		if i >= index then
			local r = executor:update(children[i])
			if r == B.Status.Working then
				return B.Status.Working
			elseif r == B.Status.Failure then
				state[self.INDEX] = nil
				return B.Status.Failure
			else
				index = index + 1
				state[self.INDEX] = index
			end
		else
			executor:visit(children[i])
		end
	end
	state[self.INDEX] = #children + 1

	local close = state[self.CLOSE]
	if not close then
		local bank = A.Cortex.get("GUI::BankWindow")
		close = A.Cortex.poke(bank, "close")
		state[self.CLOSE] = close
	end

	if A.Cortex.Result.pending(close) then
		return B.Status.Working
	else
		self:deactivate(executor)
		if A.Cortex.Result.succeeded(close) then
			return B.Status.Success
		else
			return B.Status.Failure
		end
	end
end

function UseNearbyBank:deactivated(mashina, state)
	state[self.INDEX] = nil
	state[self.CLOSE] = nil
end

function UseNearbyBank:activated(mashina, state, executor)
	local tile = state[self.TILE] or { x = state[self.X], y = state[self.Y], layer = state[self.LAYER] }
	local update = B.TreeBuilder.Root() {
		K.Selector {
			K.Check {
				condition = self.VISIBLE
			},

			K.Sequence {
				K.GUI.BankWindow.GetIsVisible {
					[self.VISIBLE] = B.Output.result,
				},

				K.Check {
					condition = self.VISIBLE
				},
			},

			K.Sequence {
				K.Game.World.FindObject { name = "features.bank" },
				
				K.Action {
					K.Game.World.GetNearbyObjects {
						name = "features.bank",
						[self.BANK_TILE] = B.Output { x = B.Output.X[1], y = B.Output.Y[1], layer = B.Output.LAYER[1] }
					},

					K.Game.World.PokeNearbyObject {
						tile = self.BANK_TILE,
						name = "features.bank",
						action = "bank"
					},

					K.GUI.BankWindow.GetIsVisible {
						[self.VISIBLE] = B.Output.result,
					},

					K.Check {
						condition = self.VISIBLE
					}
				}
			}
		}
	}

	state[self.UPDATE] = B.TreeBuilder.materialize(update)
end

return UseNearbyBank
