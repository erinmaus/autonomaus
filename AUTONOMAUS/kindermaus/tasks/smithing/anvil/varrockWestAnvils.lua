-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local T = B.Task("smithing.anvil", "Forging (Varrock West)")
T.BAR = B.Reference()
T.PRODUCT = B.Reference()
T.TERTIARY = B.Reference()
T.ANVIL_OBJECT = B.Reference()
T.BANK_OBJECT = B.Reference()
T.PROGRESS_VISIBLE = B.Reference()
T.MAKE_X_VISIBLE = B.Reference()

function T.Cost(action, count)
	return A.Game.Cost.Travel("West Bank, Varrock, Misthalin", "West Smithy, Varrock, Misthalin") +
		   A.Game.Cost.Travel("West Smithy, Varrock, Misthalin", "West Bank, Varrock, Misthalin") +
		   A.Game.Cost.Skill("smithing", count, action)
end

function T.Match(action)
	local requirements = A.Game.Requirements(action)
	local inputs = { action:inputs("item") }
	local outputs = { action:outputs("item") }

	return #inputs == 1 and #outputs == 1
end

function T:new(state, action, count)
	local requirements = A.Game.Requirements(action)
	local bar = action:input("item")
	local product = action:output("item")

	message.bar = bar.name
	message.product = product.name
	message.target_count = count

	self:instantiate(state, message)
end

function T:instantiate(state, message)
	state[self.BAR] = message.bar
	state[self.PRODUCT] = message.product

	self.target_count = message.target_count or math.huge
	self.current_count = 0
end

T.OnBegin {
	K.Action {
		K.Navigation.WalkToLocation { location = "West Bank, Varrock, Misthalin" }
	}
}

T.OnRun {
	K.Repeat {
		K.Game.World.FindObject { name = "features.anvil" },
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
						item = T.PRODUCT
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

						K.Navigation.WalkToLocation {
							disable_minimap = true,
							location = "West Bank, Varrock, Misthalin"
						}
					},

					K.Game.World.UseNearbyBank {
						K.Selector {
							K.Player.IsInventoryEmpty,

							K.GUI.BankWindow.DepositInventory
						},

						K.GUI.BankWindow.Withdraw {
							item = T.BAR,
							quantity = -2 -- transfer_all_but_one (TODO)
						}
					}
				},

				K.Selector {
					K.Step {
						K.Game.World.GetNearbyObjects {
							name = "features.anvil",
							[T.ANVIL_OBJECT] = B.Output { x = B.Output.X[1], y = B.Output.Y[1], layer = B.Output.LAYER[1] }
						},

						K.Game.View.AdjustPitch {
							angle = 145
						},

						K.Game.World.PokeNearbyObject {
							tile = T.ANVIL_OBJECT,
							name = "features.anvil",
							action = "smith"
						}
					},

					K.Step {
						K.Navigation.WalkToLocation {
							disable_minimap = true,
							location = "West Smithy, Varrock, Misthalin"
						},

						K.Game.World.GetNearbyObjects {
							name = "features.anvil",
							[T.ANVIL_OBJECT] = B.Output { x = B.Output.X[1], y = B.Output.Y[1], layer = B.Output.LAYER[1] }
						},

						K.Game.World.PokeNearbyObject {
							tile = T.ANVIL_OBJECT,
							name = "features.anvil",
							action = "smith"
						}
					}
				},

				K.Player.WaitForIdle { timeout = 4.8 }
			}
		}
	}
}
