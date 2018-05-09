// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_RENDERER_HPP
#define AUTONOMAUS_GRAPHICS_RENDERER_HPP

#include <vector>
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/immediateDispatch.hpp"

namespace autonomaus
{
	class Renderer
	{
	public:
		friend class Mesh;
		friend class Program;
		friend class Texture;

		Renderer(touchstone::ImmediateDispatch& dispatch);
		~Renderer() = default;

		void begin();
		void end();

		void set_viewport(int x, int y, int width, int height);

		void push_style();
		void pop_style();

		enum
		{
			blend_none = 0,
			blend_alpha,
			blend_additive
		};
		void set_blend_mode(int mode);

		void enable_depth_testing();
		void disable_depth_testing();

		void set_scissor(int x, int y, int width, int height);
		void unset_scissor();

		void set_line_thickness(float value);

		GLuint get_active_unit();

	private:
		touchstone::ImmediateDispatch* dispatch;

		GLint original_program = 0;
		GLint current_program = 0;
		void use_program(GLuint value);

		GLint current_element_array_buffer = 0;
		GLint original_element_array_buffer = 0;
		void bind_element_array_buffer(GLuint value);

		GLint current_array_buffer = 0;
		GLint original_array_buffer = 0;
		void bind_array_buffer(GLuint value);

		GLint current_vertex_array = 0;
		GLint original_vertex_array = 0;
		void bind_vertex_array(GLuint value);

		GLint active_unit = 0;
		GLint current_texture = 0;
		GLint original_texture = 0;
		void bind_texture(GLuint value);

		GLboolean original_depth_testing_enabled;
		GLint original_depth_testing_func;

		GLboolean original_blend_enabled;
		GLint original_blend_mode_source_color, original_blend_mode_destination_color;
		GLint original_blend_mode_source_alpha, original_blend_mode_destination_alpha;

		GLboolean original_scissor_testing_enabled;
		GLint original_scissor_rect[4];

		GLint original_viewport[4];

		GLboolean original_cull_face_enabled;
		GLboolean original_stencil_test_enabled;

		GLfloat original_line_width;

		struct Style
		{
			int blend_mode = blend_alpha;
			bool enable_depth_testing = true;
			bool scissor_set = false;
			int scissor_x = 0, scissor_y = 0;
			int scissor_width = 0, scissor_height = 0;
			float line_thickness = 1.0f;
		};
		std::vector<Style> style_stack;
	};
}

#endif
