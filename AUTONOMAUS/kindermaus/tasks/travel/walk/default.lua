-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local T = B.Task("travel.walk", "Walk (Default)")
T.DESTINATION = B.Local()
T.DESTINATION_X = B.Local()
T.DESTINATION_Y = B.Local()
T.DESTINATION_LAYER = B.Local()
T.IS_WALKING = B.Local()
T.AT_DESTINATION = B.Local()

function T.Cost(action)
	return A.Game.Cost.Travel(action.cost_multiplier or 1)
end

function T:new(state, action, count)
	local requirements = A.Game.Requirements(action)
	local destination = requirements:resource("location")

	self:instantiate(state, { location = destination.address })
end

function T:instantiate(state, message)
	local destination = A.Game.Atlas.find(message.location)[1]

	state[self.DESTINATION] = destination.address
	state[self.DESTINATION_X] = destination.anchor.x
	state[self.DESTINATION_Y] = destination.anchor.y
	state[self.DESTINATION_LAYER] = destination.anchor.layer
end

function T:run(executor)
	return self.super.run(self, executor)
end

T.OnBegin {
	K.Game.Walker.FindPath {
		x = T.DESTINATION_X,
		y = T.DESTINATION_Y,
		layer = T.DESTINATION_LAYER
	}
}

T.OnRun {
	K.Sequence {
		K.Repeat {
			K.Game.Walker.GetPending {
				[T.IS_WALKING] = B.Output.RESULT
			},

			K.Check {
				condition = T.IS_WALKING,
			},

			K.Invert {
				K.Navigation.IsAtLocation {
					location = T.DESTINATION
				}
			},

			K.Selector {
				K.Sequence {
					K.Game.Walker.GetAtDestination {
						distance = 1,
						[T.AT_DESTINATION] = B.Output.RESULT
					},

					K.Check { condition = T.AT_DESTINATION },

					K.Game.Walker.Finish
				},

				K.Try {
					K.Sequence {
						K.Game.Walker.Step,

						K.QuickIdle
					},

					K.Try {
						K.Game.Walker.Step,

						K.Step {
							K.Game.Walker.FindPath {
								x = T.DESTINATION_X,
								y = T.DESTINATION_Y,
								layer = T.DESTINATION_LAYER
							},

							K.Game.Walker.Step
						}
					}
				}
			}
		},

		K.Navigation.IsAtLocation {
			location = T.DESTINATION
		}
	}
}
