// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_GRAPHICS_GUI_MANAGER_HPP
#define PRINCIPLE_GRAPHICS_GUI_MANAGER_HPP

#include "principle/deps/imgui/imgui.h"
#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvncer/graphics/shader.hpp"
#include "kvncer/graphics/texture.hpp"

namespace principle
{
	class Display;

	class GUIManager
	{
	public:
		GUIManager(const Display* display);
		~GUIManager();

		void start(float timestep);
		void stop();
		void render();
		void render(ImDrawList& draw_list);

		void send_text_input(unsigned int c);

	private:
		const Display* display;
		std::unique_ptr<kvncer::Shader> shader;
		std::unique_ptr<kvncer::Texture> font;
		std::unique_ptr<kvncer::Mesh> mesh;
		std::unique_ptr<kvncer::MeshBuffer> index_buffer;
		std::unique_ptr<kvncer::MeshBuffer> vertex_buffer;

		int previous_mouse_z = 0;
	};
}

#endif
