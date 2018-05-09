// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_QUERY_MATERIAL_DEFINITION_HPP
#define CHOMSKY_WORLD_QUERY_MATERIAL_DEFINITION_HPP

#include <glm/glm.hpp>
#include "chomsky/resources/textureFingerprint.hpp"

namespace chomsky
{
	class MaterialDefinition
	{
	public:
		MaterialDefinition() = default;
		~MaterialDefinition() = default;

		const TextureFingerprint& get_fingerprint() const;
		void set_fingerprint(const TextureFingerprint& value);

		const glm::vec3& get_max_difference_threshold(void) const;
		void set_max_difference_threshold(const glm::vec3& value);

		const glm::vec3& get_max_dissimilarity(void) const;
		void set_max_dissimilarity(const glm::vec3& value);
		
		const glm::vec3& get_color() const;
		void set_color(const glm::vec3& value);

		const glm::vec3& get_max_color_difference() const;
		void set_max_color_difference(const glm::vec3& value);

	private:
		TextureFingerprint fingerprint;
		glm::vec3 max_difference_threshold = glm::vec3(0.0f);
		glm::vec3 max_dissimilarity = glm::vec3(0.0f);

		glm::vec3 color = glm::vec3(0.0f);
		glm::vec3 max_color_difference = glm::vec3(0.0f);
	};
}

#endif
