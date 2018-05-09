-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local PokeNearbyObject = B.Node("Game.World.PokeNearbyObject")
PokeNearbyObject.TILE = B.Reference()
PokeNearbyObject.X = B.Reference()
PokeNearbyObject.Y = B.Reference()
PokeNearbyObject.LAYER = B.Reference()
PokeNearbyObject.NAME = B.Reference()
PokeNearbyObject.ACTION = B.Reference()
PokeNearbyObject.UPDATE = B.Local()

function PokeNearbyObject:update(mashina, state, executor)
	return state[self.UPDATE]:execute(executor)
end

function PokeNearbyObject:activated(mashina, state, executor)
	local tile = state[self.TILE] or { x = state[self.X], y = state[self.Y], layer = state[self.LAYER] }
	local update = B.TreeBuilder.Root() {
		K.Step {
			K.Selector {
				K.Navigation.WithinRadius {
					tile = tile,
					radius = 4
				},

				K.Game.View.FocusTile {
					x = tile.x,
					y = tile.y,
					layer = tile.layer,
					range = 90
				}
			},

			K.Player.WaitForIdle { timeout = 0.0 },

			K.Cortex.Game.World.PokeNearbyObject {
				x = tile.x,
				y = tile.y,
				layer = tile.layer,
				name = state[self.NAME],
				action = state[self.ACTION]
			},

			K.Player.WaitForIdle { timeout = 0.3 }
		}
	}

	state[self.UPDATE] = B.TreeBuilder.materialize(update)
end

return PokeNearbyObject
