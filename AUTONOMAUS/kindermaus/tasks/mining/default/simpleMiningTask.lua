-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

return function(name, bank, mine)
	local T = B.Task("mining.default", name)
	T.RESOURCE = B.Reference()
	T.ROCK_OBJECT = B.Reference()
	T.NEXT_ROCK = B.Reference()
	T.QUANTITY = B.Reference()
	T.RESOURCE_COUNT = B.Reference()

	local super = T.super
	T.super = T

	function T.Cost(action, count)
		return A.Game.Cost.Travel(bank, mine) +
		       A.Game.Cost.Travel(mine, bank) +
		       A.Game.Cost.Skill("mining", count, action)
	end

	function T:new(state, action, count)
		local requirements = A.Game.Requirements(action)
		local ore = action:output("item")

		self:instantiate(state, { target_count = count, resource = ore.name })
	end

	function T:instantiate(state, message)
		self.target_quantity = message.target_count or math.huge
		self.previous_quantity = 0

		state[self.RESOURCE] = message.resource
		state[self.RESOURCE_COUNT] = 0

		local Resources = {
			["Tin ore"] = "mining.rocks.tin",
			["Copper ore"] = "mining.rocks.copper",
			["Iron ore"] = "mining.rocks.iron",
			["Coal"] = "mining.rocks.coal",
			["Mithril ore"] = "mining.rocks.mithril",
			["Adamantite ore"] = "mining.rocks.adamantite",
			["Runite ore"] = "mining.rocks.runite"
		}

		if not Resources[message.resource] then
			errorf("unknown mining resource '%s'", message.resource)
		end

		state[self.ROCK_OBJECT] = Resources[message.resource]
	end

	function T:run(executor)
		local state = executor:state()
		local quantity = state[self.QUANTITY] or 0
		if quantity ~= self.previous_quantity then
			A.Log.info("(Task) Mined %d %s.", quantity, state[self.RESOURCE] or "???")
			self.previous_quantity = quantity
		end

		if (state[self.QUANTITY] or 0) >= self.target_quantity then
			return B.Status.Success
		else
			return super.run(self, executor)
		end
	end

	T.OnBegin {
		K.Action {
			K.Action {
				K.Navigation.WalkToLocation { location = bank },

				K.Game.World.UseNearbyBank {
					K.Selector {
						K.Player.IsInventoryEmpty,
						K.GUI.BankWindow.DepositInventory
					}
				}
			},

			K.Action {
				K.Navigation.WalkToLocation { location = mine }
			}
		}
	}

	T.OnRun {
		K.Repeat {
			K.Game.World.FindObject { name = T.ROCK_OBJECT },

			K.Selector {
				K.Step {
					K.Player.IsInventoryFull,

					K.Idle,

					K.Action {
						K.Player.CountInventory {
							item = T.RESOURCE,
							[T.RESOURCE_COUNT] = B.Output.RESULT
						},

						K.Add {
							left = T.RESOURCE_COUNT,
							right = T.QUANTITY,
							[T.QUANTITY] = B.Output.RESULT
						},

						K.Navigation.WalkToLocation { location = bank },

						K.Game.World.UseNearbyBank {
							K.GUI.BankWindow.DepositInventory
						}
					},

					K.Action {
						K.Navigation.WalkToLocation { location = mine }
					},

					K.Yield
				},

				K.Selector {
					K.Sequence {
						K.Game.World.GetNearbyObjects {
							name = T.ROCK_OBJECT
						},

						K.Action {
							K.Game.World.GetNearbyObjects {
								name = T.ROCK_OBJECT,
								[T.NEXT_ROCK] = B.Output { x = B.Output.X[1], y = B.Output.Y[1], layer = B.Output.LAYER[1] }
							},

							K.Game.World.DepleteResource {
								tile = T.NEXT_ROCK,
								name = T.ROCK_OBJECT,
								action = "mine",
							}
						}
					},

					K.Step {
						K.Action {
							K.Game.World.GetNearbyObjects {
								visible = false,
								name = T.ROCK_OBJECT,
								[T.NEXT_ROCK] = B.Output { x = B.Output.X[1], y = B.Output.Y[1], layer = B.Output.LAYER[1] }
							},

							--K.Navigation.WalkToNearbyObject {
							--	tile = T.NEXT_ROCK
							--},

							K.Repeat {
								K.Invert {
									K.Game.World.GetNearbyObjects {
										name = T.ROCK_OBJECT
									}
								},

								K.Idle
							}
						}
					}
				}
			}
		}
	}

	return T
end
