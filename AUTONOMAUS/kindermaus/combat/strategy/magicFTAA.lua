-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local MagicFTAA = B.Node("Combat.Strategy.MagicFTAA")
MagicFTAA.INCLUDE = B.Reference()
MagicFTAA.EXCLUDE = B.Reference()
MagicFTAA.MAIN_HAND = B.Reference()
MagicFTAA.OFF_HAND = B.Reference()
MagicFTAA.STAFF = B.Reference()
MagicFTAA.SPELL = B.Reference()
MagicFTAA.NEXT_ABILITY = B.Local()
MagicFTAA.UPDATE = B.Local()

function MagicFTAA:update(mashina, state, executor)
	if not state[self.UPDATE] then
		self:deactivate(executor)
		return B.Status.Failure
	end

	return state[self.UPDATE]:execute(executor)
end

function MagicFTAA:activated(mashina, state, executor)
	local include = state[self.INCLUDE] or A.Game.Data.Rotations["magic"]
	local exclude = state[self.EXCLUDE] or {}
	local abilities = {}
	for i = 1, #include do
		local add = true
		for j = 1, #exclude do
			if exclude[j] == include[i] then
				add = false
				break
			end
		end

		if add then
			table.insert(abilities, include[i])
		end
	end

	local update = B.TreeBuilder.Root() {
		K.Step {
			K.Delay {
				timeout = 0.6,

				K.Sequence {
					K.Selector {
						K.Invert {
							K.Player.HasInventoryItem { item = state[self.MAIN_HAND] },
						},

						K.Player.EquipItem { item = state[self.MAIN_HAND] }
					},

					K.Selector {
						K.Invert {
							K.Player.HasInventoryItem { item = state[self.OFF_HAND] },
						},

						K.Player.EquipItem { item = state[self.OFF_HAND] }
					}
				}
			},

			K.Print { message = "Wand/orb" },

			K.Action {
				K.Combat.NextAbility {
					abilities = abilities,
					[self.NEXT_ABILITY] = B.Output.RESULT
				},

				K.Combat.UseAbility {
					ability = self.NEXT_ABILITY,
				}
			},

			K.Print { message = "Wand/orb ability" },

			K.Delay {
				timeout = 1.8,

				K.Action {
					K.Player.EquipItem { item = state[self.STAFF] }
				}
			},

			K.Print { message = "Staff" },

			K.Success {
				K.GUI.ActionBar.Activate {
					type = A.Cortex.GUI.ActionBar.type_spell,
					value = self.SPELL
				},
			},

			K.Print { message = "Spell" },

			K.Action {
				K.Combat.NextAbility {
					abilities = abilities,
					[self.NEXT_ABILITY] = B.Output.RESULT
				},

				K.Combat.UseAbility {
					ability = self.NEXT_ABILITY,
					queue = false
				}
			},

			K.Print { message = "Staff ability" },
		}
	}

	state[self.UPDATE] = B.TreeBuilder.materialize(update)
end

return MagicFTAA
