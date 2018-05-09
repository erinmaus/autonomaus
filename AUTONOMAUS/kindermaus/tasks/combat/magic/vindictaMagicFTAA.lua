-- This file is a part of AUTONOMAUS.
--
-- Look, but don't touch.
--
-- Copyright 2017 [bk]door.maus

local FSM = require "autonomaus.deps.fsm"

local T = B.Task("combat.magic", "Vindicta (Magic FTAA)")
T.MAIN_HAND = B.Local()
T.OFF_HAND = B.Local()
T.STAFF = B.Local()
T.SPELL = B.Local()
T.VINDICTA = B.Local()
T.FIRE = B.Local()
T.UPDATE = B.Local()
T.NEXT_ATTACK = B.Local()

T.Phase1 = {}
function T.Phase1:update(state)
	self.animations.phase_1_attack:update()

	local target = A.Cortex.get("GUI::Target")
	local is_visible = A.Cortex.pull(target, "is_visible")
	if is_visible and is_visible.result then
		local current_health = A.Cortex.pull(target, "current_health")
		if current_health and current_health.result <= 100000 then
			print("PHASE 1 SUCCESS")
		end
	end
end

function T.Phase1:idle(state)
	state[self.NEXT_ATTACK] = "melee"

	self:transition("before_hurricane")
end

function T.Phase1:before_hurricane(state)
	if self.spawned then
		if self.animations.phase_1_attack:match(1) then
			print "spawned phase_1_attack 2"
			self:transition("hurricane")
		end
	else
		if self.animations.phase_1_attack:match(3) then
			print "after phase_1_attack 3"
			self:transition("hurricane")
		end
	end
end

function T.Phase1:before_fire(state)
	if self.spawned then
		if self.animations.phase_1_attack:match(3) then
			print "spawned phase_1_attack 5"
			self:transition("summon")
			self.animations.phase_1_attack:reset()
			self.spawned = false
		end
	else
		if self.animations.phase_1_attack:match(5) then
			print "after phase_1_attack 6"
			self:transition("summon")
			self.animations.phase_1_attack:reset()
		end
	end
end

function T.Phase1:hurricane(state)
	self.animations.phase_1_hurricane:update()

	state[self.NEXT_ATTACK] = "hurricane"

	if self.animations.phase_1_hurricane:match(1) then
		self:transition("before_fire")
		self.animations.phase_1_hurricane:reset()
	end
end

function T.Phase1:summon(state)
	self.animations.phase_1_summon:update()

	if self.animations.phase_1_summon:match(1) then
		self:transition("fire")
		self.animations.phase_1_summon:reset()
	end
end

function T.Phase1:fire(state)
	state[self.NEXT_ATTACK] = "fire"

	if self.spawned then
		if self.animations.phase_1_attack:match(4) then
			self:transition("before_hurricane")
			self.spawned = false
		end
	else
		if self.animations.phase_1_attack:match(1) then
			self:transition("before_hurricane")
		end
	end
end

function T:transition(state)
	local p
	if self.phase == 1 then
		p = self.Phase1
	elseif self.phase == 2 then
		p = self.Phase2
	end

	if p and p[state] then
		A.Log.info("Transitioned from %s to %s.", self.state, state)
		self.state = state
	else
		A.Log.warn("State '%s' not valid in phase %d.", tostring(state), self.phase)
		self.state = "idle"
	end
end

function T:instantiate(state, message)
	state[self.MAIN_HAND] = message.main_hand
	state[self.OFF_HAND] = message.off_hand
	state[self.STAFF] = message.staff
	state[self.SPELL] = message.spell

	self.phase = 1
	self.state = "idle"
	self.spawned = true

	self.animations = {
		phase_1_attack = A.Game.Combat.Animation("mobs.bosses.vindicta", "attack"),
		phase_1_hurricane = A.Game.Combat.Animation("mobs.bosses.vindicta", "spin"),
		phase_1_summon = A.Game.Combat.Animation("mobs.bosses.vindicta", "summon"),
		phase_2_attack = A.Game.Combat.Animation("mobs.bosses.gorvek_and_vindicta", "attack")
	}

	local world = A.Cortex.get("Game::World")
	self.resources = {
		vindicta = A.Cortex.poke(world, "find_npc", { name = "mobs.bosses.vindicta" }),
		gorvek = A.Cortex.poke(world, "find_npc", { name = "mobs.bosses.gorvek_and_vindicta" }),
		fire = A.Cortex.poke(world, "find_object", { name = "combat.bosses.vindicta.fire" }),
	}
end

function T:run(executor)
	local state = executor:state()

	local p
	if self.phase == 1 then
		p = self.Phase1
		--state[self.UPDATE] = self.OnPhase1
	elseif self.phase == 2 then
		p = self.Phase2
		--state[self.UPDATE] = self.OnPhase2
	end

	if p then
		if p.update then
			p.update(self, state)
		end

		if p[self.state] then
			p[self.state](self, state)
		else
			print "no state"
		end
	else
		print "no p"
	end

	return self.super.run(self, executor)
end

if false then
T.OnBegin {
	K.Action {
		K.Game.World.GetNearbyNPCS {
			name = "mobs.bosses.vindicta",
			[T.VINDICTA] = B.Output.ID[1]
		},

		K.Game.World.PokeNearbyNPC {
			id = T.VINDICTA,
			action = "attack"
		}
	}
}
end

T.OnPhase1 = B.TreeBuilder.Root() {
	K.Sequence {
		K.Selector {
			K.Sequence {
				K.Equal {
					a = T.NEXT_ATTACK,
					b = "hurricane"
				},

				K.Player.UsePrayer {
					prayer = "deflect melee"
				}
			},

			K.Player.UsePrayer {
				prayer = "soul split"
			}
		},

		K.Combat.Strategy.MagicFTAA {
			main_hand = T.MAIN_HAND,
			off_hand = T.OFF_HAND,
			staff = T.STAFF,
			spell = T.SPELL
		}
	}
}

T.OnRun {
	K.Repeat {
		--K.Sequence {
		--	K.Game.World.GetNearbyObjects {
		--		[T.FIRE] = B.Output { x = B.Output.X, y = B.Output.Y, layer = B.Output.LAYER }
		--	},

		--	K.Navigation.AvoidTiles {
		--		tiles = T.FIRE
		--	}
		--},

		--K.Combat.Heal {
		--	threshold = 3000,
		--},

		--K.Combat.RestorePrayer,

		K.Action {
			K.Execute {
				tree = T.UPDATE
			}
		}
	}
}
