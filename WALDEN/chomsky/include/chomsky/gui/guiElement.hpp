// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GUI_ELEMENT_HPP
#define CHOMSKY_GUI_GUI_ELEMENT_HPP

#include <string>
#include <glm/glm.hpp>

namespace chomsky
{
	class GUIElement
	{
	public:
		static const std::size_t NUM_VERTICES = 4;
		static const std::size_t TOP_LEFT = 0;
		static const std::size_t TOP_RIGHT = 1;
		static const std::size_t BOTTOM_RIGHT = 2;
		static const std::size_t BOTTOM_LEFT = 3;

		struct Vertex
		{
			glm::ivec2 position;
			glm::vec2 texture_coordinate;
			glm::vec2 wrapped_texture_coordinate;
			glm::vec2 atlas_min;
			glm::vec2 atlas_max;
			glm::vec4 color;
		};

		GUIElement() = default;
		GUIElement(
			const Vertex vertices[], std::size_t num_vertices,
			std::size_t offset = 0,
			const std::string& key = std::string(),
			int texture_name = 0, int texture_region_name = 0, bool is_textured = false);
		~GUIElement() = default;

		std::size_t get_num_vertices() const;
		Vertex get_vertex(std::size_t index) const;

		std::size_t get_offset() const;

		int get_x() const;
		int get_y() const;
		int get_width() const;
		int get_height() const;

		float get_rotation() const;

		const std::string& get_key() const;
		void set_key(const std::string& value);

		int get_texture_name() const;
		int get_texture_region_name() const;
		bool get_is_textured() const;

		glm::vec4 get_color() const;

	private:
		Vertex vertices[NUM_VERTICES];
		std::size_t num_vertices = NUM_VERTICES;

		std::size_t offset = 0;

		int x = 0, y = 0;
		int width = 0, height = 0;
		float rotation = 0.0f;

		std::string key;
		bool is_textured = false;
		int texture_name = 0, texture_region_name = 0;
		glm::vec4 color = glm::vec4(0.0f);
	};
}

#endif
