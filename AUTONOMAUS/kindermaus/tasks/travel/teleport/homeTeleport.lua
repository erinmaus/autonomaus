-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local T = B.Task("travel.teleport", "Home Teleport")
T.DESTINATION = B.Local()
T.AT_DESTINATION = B.Local()

function T.Cost(action)
	return A.Game.Cost.Travel(action.cost_multiplier or 1)
end

function T.Match(action)
	local name_pattern = "(.+) Home Teleport"
	local destination = action.resource.name:match(name_pattern)
	if destination then
		return true
	end
end

function T:new(state, action, count)
	local name_pattern = "(.+) Home Teleport"
	local destination = action.resource.name:match(name_pattern)

	if not destination then
		errorf("destination not found; action (%d) for '%s' isn't a home teleport", action.id, action.resource.name)
	end

	self:instantiate(state, { destination = destination })
end

function T:instantiate(state, message)
	state[self.DESTINATION] = message.destination
end

T.OnRun {
	K.Step {
		K.Player.IsIdle,

		K.Try {
			K.GUI.HomeTeleportWindow.Teleport {
				mode = A.Cortex.GUI.HomeTeleportWindow.teleport_quick,
				destination = T.DESTINATION
			},

			K.Sequence {
				K.Player.IsIdle,

				K.GUI.HomeTeleportWindow.Teleport {
					mode = A.Cortex.GUI.HomeTeleportWindow.teleport_quick,
					destination = T.DESTINATION
				}
			}
		},

		K.Selector {
			K.Player.WaitForIdle { timeout = 3 },

			K.Navigation.IsAtLocation {
				location = T.DESTINATION
			}
		},

		K.Navigation.IsAtLocation {
			location = T.DESTINATION
		}
	}
}
