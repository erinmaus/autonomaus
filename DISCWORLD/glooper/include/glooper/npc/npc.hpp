// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_NPC_NPC_HPP
#define GLOOPER_NPC_NPC_HPP

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <set>
#include "kvlc/geometry/bounds.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace glooper
{
	class NPC
	{
	public:
		typedef std::uint32_t ID;
		static const ID ID_NONE = 0;
		static const ID ID_BASE = 1;

		NPC() = default;
		explicit NPC(ID id);
		NPC(ID id, const NPC& other);
		~NPC() = default;

		ID get_id() const;

		const std::string& get_profile_name() const;
		void set_profile_name(const std::string& value);

		const std::string& get_profile_variant() const;
		void set_profile_variant(const std::string& value);

		const std::set<std::string>& get_animations() const;
		void set_animations(const std::set<std::string>& value);

		const glm::vec3& get_position() const;
		void set_position(const glm::vec3& value);

		const glm::vec3& get_previous_position() const;
		void set_previous_position(const glm::vec3& value);

		const thoreau::WorldCoordinate& get_world_coordinate() const;
		void set_world_coordinate(const thoreau::WorldCoordinate& value);

		const kvlc::Bounds& get_bounds() const;
		void set_bounds(const kvlc::Bounds& value);

		const glm::mat4& get_model_matrix() const;
		void set_model_matrix(const glm::mat4& value);

		bool get_is_on_screen() const;
		void set_is_on_screen(bool value);

		bool is_visible(const glm::mat4& view, const glm::mat4& projection) const;
		bool maybe_on_screen(const glm::mat4& view, const glm::mat4& projection) const;

	private:
		ID id = ID_NONE;
		std::string profile_name;
		std::string profile_variant;

		glm::vec3 previous_position = glm::vec3(0.0f);
		glm::vec3 position = glm::vec3(0.0f);
		thoreau::WorldCoordinate world_coordinate = thoreau::WorldCoordinate(0, 0);
		kvlc::Bounds bounds = kvlc::Bounds(glm::vec3(0.0f), glm::vec3(0.0f));
		glm::mat4 model_matrix = glm::mat4(1.0f);

		std::set<std::string> animations;

		bool is_on_screen = false;
	};
}

#endif
