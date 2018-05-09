-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local DepleteResource = B.Node("Game.World.DepleteResource")
DepleteResource.TILE = B.Reference()
DepleteResource.NAME = B.Reference()
DepleteResource.ACTION = B.Reference()
DepleteResource.UPDATE = B.Local()

function DepleteResource:update(mashina, state, executor)
	return state[self.UPDATE]:execute(executor)
end

function DepleteResource:activated(mashina, state, executor)
	local tile = state[self.TILE] or { x = state[self.X], y = state[self.Y], layer = state[self.LAYER] }
	local update = B.TreeBuilder.Root() {
		K.Step {
			K.Game.World.PokeNearbyObject {
				tile = tile,
				name = state[self.NAME],
				action = state[self.ACTION]
			},

			K.Repeat {
				K.Game.World.IsResourceVisible {
					tile = tile,
					name = state[self.NAME]
				},

				K.Invert {
					K.Player.WaitForIdle {
						timeout = 0.6
					}
				}
			},

			K.Invert {
				K.Game.World.IsResourceVisible {
					tile = tile,
					name = state[self.NAME]
				}
			}
		}
	}

	state[self.UPDATE] = B.TreeBuilder.materialize(update)
end

return DepleteResource
