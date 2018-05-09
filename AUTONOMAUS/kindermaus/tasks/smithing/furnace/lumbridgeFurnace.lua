-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local T = B.Task("smithing.furnace", "Ore Smelting (Lumbridge Furnace)")
T.ORE = B.Reference()
T.TERTIARY = B.Reference()
T.COUNT = B.Reference()
T.BAR = B.Reference()
T.FURNACE_OBJECT = B.Reference()
T.BANK_OBJECT = B.Reference()
T.PROGRESS_VISIBLE = B.Reference()
T.MAKE_X_VISIBLE = B.Reference()
T.INVENTORY_COUNT = B.Reference()
T.BAR_COUNT = B.Reference()

function T.Cost(action, count)
	return A.Game.Cost.Travel("Combat Academy, Lumbridge, Misthalin", "Smithy, Lumbridge, Misthalin") +
		   A.Game.Cost.Travel("Smithy, Lumbridge, Misthalin", "Combat Academy, Lumbridge, Misthalin") +
		   A.Game.Cost.Skill("smithing", count, action)
end

local function get_ores(action)
	local ores = { action:inputs("item") }

	local result = { resource = {}, count = {} }
	for i = 1, #ores do
		if ores:match(".*([Oo]re)$") then
			table.insert(result.resource, ores[i].name)
			table.insert(result.count, ores[i].requirements.count)
		end
	end

	return result
end

function T.Match(action)
	local ores = get_ores(action)
	return #ores.names <= 2
end

function T:new(state, action, count)
	local requirements = A.Game.Requirements(action)
	local bar = action:output("item")
	local message = get_ores(action)
	message.bar = bar.name
	message.target_count = count

	self:instantiate(state, message)
end

function T:instantiate(state, message)
	local primary, tertiary, total
	if #message.resource == 1 then
		primary = 1
		total = message.count[1]
	else
		if message.count[1] <= message.count[2] then
			primary = 1
			tertiary = 2
		else
			primary = 2
			tertiary = 1
		end
		total = message.count[1] + message.count[2]
	end

	-- TODO: Used named constant instead of hard-coding inventory count.
	if total == 1 then
		state[self.COUNT] = -2 -- transfer_all_but_one (TODO)
	else
		state[self.COUNT] = math.floor(28 / total)
	end

	state[self.ORE] = message.resource[primary]
	if tertiary then
		state[self.TERTIARY] = message.resource[tertiary]
	end

	state[self.BAR] = message.bar

	self.target_quantity = message.target_count or math.huge
	self.previous_quantity = 0
end

function T:run(executor)
	local state = executor:state()
	local quantity = state[self.BAR_COUNT] or 0
	if quantity ~= self.previous_quantity then
		A.Log.info("(Task) Smelted %d %s.", quantity, state[self.BAR] or "???")
		self.previous_quantity = quantity
	end

	if (state[self.BAR_COUNT] or 0) >= self.target_quantity then
		return B.Status.Success
	else
		return self.super.run(self, executor)
	end
end

T.OnBegin {
	K.Action {
		K.Navigation.WalkToLocation { location = "Combat Academy, Lumbridge, Misthalin" }
	}
}

T.OnRun {
	K.Repeat {
		K.Game.World.FindObject { name = "features.furnace" },
		K.Game.World.FindObject { name = "features.bank" },

		K.Selector {
			K.Sequence {
				K.GUI.ProgressWindow.GetIsVisible {
					[T.PROGRESS_VISIBLE] = B.Output.result,
				},

				K.Check {
					condition = T.PROGRESS_VISIBLE
				}
			},

			K.Step {
				K.GUI.MakeXWindow.GetIsVisible {
					[T.MAKE_X_VISIBLE] = B.Output.RESULT
				},

				K.Check { condition = T.MAKE_X_VISIBLE },

				K.Step {
					K.GUI.MakeXWindow.Select {
						item = T.BAR
					},

					K.Selector {
						K.GUI.MakeXWindow.Make,

						K.GUI.MakeXWindow.Close
					}
				},

				K.Player.WaitForIdle {
					timeout = 1.2
				}
			},

			K.Step {
				K.Action {
					K.Selector {
						K.Step {
							K.Game.World.GetNearbyObjects {
								name = "features.bank",
								[T.BANK_OBJECT] = B.Output { x = B.Output.X[1], y = B.Output.Y[1], layer = B.Output.LAYER[1] }
							},

							K.Game.View.AdjustPitch {
								angle = 145
							},

							K.Game.World.PokeNearbyObject {
								tile = T.BANK_OBJECT,
								name = "features.bank",
								action = "bank"
							}
						},

						K.Navigation.WalkToLocation { location = "Combat Academy, Lumbridge, Misthalin" }
					},

					K.Game.World.UseNearbyBank {
						K.Player.CountInventory {
							item = T.BAR,
							[T.INVENTORY_COUNT] = B.Output.RESULT,
						},

						K.Add {
							left = T.INVENTORY_COUNT,
							right = T.BAR_COUNT,
							[T.BAR_COUNT] = B.Output.RESULT
						},

						K.Selector {
							K.Player.IsInventoryEmpty,

							K.GUI.BankWindow.DepositInventory
						},

						K.GUI.BankWindow.Withdraw {
							item = T.ORE,
							quantity = T.COUNT
						},

						K.Selector {
							K.Invert {
								K.Check { condition = T.TERTIARY }
							},

							K.GUI.BankWindow.Withdraw {
								item = T.TERTIARY,
								quantity = -2 -- transfer_all_but_one (TODO)
							}
						}
					}
				},

				K.Selector {
					K.Step {
						K.Game.World.GetNearbyObjects {
							name = "features.furnace",
							[T.FURNACE_OBJECT] = B.Output { x = B.Output.X[1], y = B.Output.Y[1], layer = B.Output.LAYER[1] }
						},

						K.Game.View.AdjustPitch {
							angle = 145
						},

						K.Game.World.PokeNearbyObject {
							tile = T.FURNACE_OBJECT,
							name = "features.furnace",
							action = "smelt"
						}
					},

					K.Step {
						K.Navigation.WalkToLocation { location = "Smithy, Lumbridge, Misthalin" },

						K.Game.World.GetNearbyObjects {
							name = "features.furnace",
							[T.FURNACE_OBJECT] = B.Output { x = B.Output.X[1], y = B.Output.Y[1], layer = B.Output.LAYER[1] }
						},

						K.Game.World.PokeNearbyObject {
							tile = T.FURNACE_OBJECT,
							name = "features.furnace",
							action = "smelt"
						}
					}
				},

				K.Player.WaitForIdle { timeout = 4.8 }
			}
		}
	}
}
