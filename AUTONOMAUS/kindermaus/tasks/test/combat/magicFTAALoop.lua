-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local T = B.Task("test.combat", "Test (Combat, Magic FTAA Loop)")
T.NPC_ID = B.Local()

function T.Cost(action)
	return 0
end

function T:new(state, action, count)
	self:instantiate(state)
end

function T:instantiate(state, message)
	-- Nothing.
end

function T:run(executor)
	return self.super.run(self, executor)
end

T.OnBegin {
	K.Sequence {
		K.Navigation.GoToLocation {
			location = "Combat Academy, Lumbridge, Misthalin"
		},

		K.Game.World.FindNPC { name = "mobs.mechanical.dummy" },

		K.Wait {
			timeout = 10,

			K.Step {
				K.Game.World.GetNearbyNPCS {
					name = "mobs.mechanical.dummy",
					[T.NPC_ID] = B.Output.ID[1]
				},

				K.Game.World.PokeNearbyNPC {
					id = T.NPC_ID,
					action = "attack"
				}
			}
		}
	}
}

T.OnRun {
	K.Repeat {
		K.Combat.Strategy.MagicFTAA {
			main_hand = "Seasinger kiba",
			off_hand = "Seasinger makigai",
			staff = "Noxious staff",
			spell = "Fire surge"
		},

		K.Print { message = "Success!" },

		K.Wait { timeout = 1.8 }
	}
}
