// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstddef>
#include <allegro5/allegro.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "principle/graphics/display.hpp"
#include "principle/graphics/guiManager.hpp"
#include "principle/deps/imgui/imgui.h"

static const char* VertexShaderSource =
	"#version 140\n"
	"uniform mat4 principle_Projection;\n"
	"in vec2 vert_Position;\n"
	"in vec2 vert_Texture;\n"
	"in vec4 vert_Color;\n"
	"out vec2 frag_Texture;\n"
	"out vec4 frag_Color;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(vert_Position, 0.0, 1.0) * principle_Projection;\n"
	"	frag_Texture = vec2(vert_Texture.s, vert_Texture.t);\n"
	"	frag_Color = vert_Color;\n"
	"}\n";

static const char* FragmentShaderSource =
	"#version 140\n"
	"uniform sampler2D principle_Texture;\n"
	"in vec2 frag_Texture;\n"
	"in vec4 frag_Color;\n"
	"out vec4 out_Color;\n"
	"void main()\n"
	"{\n"
	"	out_Color = frag_Color * texture(principle_Texture, frag_Texture);\n"
	"}\n";

static void set_clipboard_text(void*, const char* text)
{
	al_set_clipboard_text(al_get_current_display(), text);
}

static const char* get_clipboard_text(void*)
{
	return al_get_clipboard_text(al_get_current_display());
}

principle::GUIManager::GUIManager(const Display* display)
{
	this->display = display;

	auto& io = ImGui::GetIO();

	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	this->font = std::make_unique<kvncer::Texture>(
		kvncer::Texture::component_format_rgba, false,
		width, height);
	this->font->update(
		0, 0, width, height,
		kvncer::Texture::component_format_rgba,
		kvncer::Texture::component_type_unsigned_byte,
		pixels);
	io.Fonts->TexID = this->font.get();

	this->shader = std::make_unique<kvncer::Shader>(
		VertexShaderSource, FragmentShaderSource);
	this->shader->bind_input("vert_Position", kvncer::Shader::vertex_input_position);
	this->shader->bind_input("vert_Texture", kvncer::Shader::vertex_input_texture_coordinate);
	this->shader->bind_input("vert_Color", kvncer::Shader::vertex_input_color);

	this->index_buffer = std::make_unique<kvncer::MeshBuffer>();
	this->vertex_buffer = std::make_unique<kvncer::MeshBuffer>();

	this->mesh = std::make_unique<kvncer::Mesh>();

	io.KeyMap[ImGuiKey_Tab] = ALLEGRO_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = ALLEGRO_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = ALLEGRO_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = ALLEGRO_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = ALLEGRO_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = ALLEGRO_KEY_PGUP;
	io.KeyMap[ImGuiKey_PageDown] = ALLEGRO_KEY_PGDN;
	io.KeyMap[ImGuiKey_Home] = ALLEGRO_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = ALLEGRO_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = ALLEGRO_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = ALLEGRO_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = ALLEGRO_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = ALLEGRO_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = ALLEGRO_KEY_A;
	io.KeyMap[ImGuiKey_C] = ALLEGRO_KEY_C;
	io.KeyMap[ImGuiKey_V] = ALLEGRO_KEY_V;
	io.KeyMap[ImGuiKey_X] = ALLEGRO_KEY_X;
	io.KeyMap[ImGuiKey_Y] = ALLEGRO_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = ALLEGRO_KEY_Z;

	io.SetClipboardTextFn = &set_clipboard_text;
	io.GetClipboardTextFn = &get_clipboard_text;
}

principle::GUIManager::~GUIManager()
{
	ImGui::Shutdown();
}

void principle::GUIManager::start(float timestep)
{
	auto& io = ImGui::GetIO();

	io.DisplaySize = ImVec2(this->display->get_width(), this->display->get_height());
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	io.DeltaTime = timestep;

	ALLEGRO_MOUSE_STATE mouse_state;
	al_get_mouse_state(&mouse_state);
	io.MousePos = ImVec2(mouse_state.x, mouse_state.y);
	io.MouseWheel = mouse_state.z - this->previous_mouse_z;
	io.MouseDown[0] = mouse_state.buttons & (1 << 0);
	io.MouseDown[1] = mouse_state.buttons & (1 << 1);
	io.MouseDown[2] = mouse_state.buttons & (1 << 2);
	this->previous_mouse_z = mouse_state.z;

	ALLEGRO_KEYBOARD_STATE keyboard_state;
	al_get_keyboard_state(&keyboard_state);
	io.KeysDown[ALLEGRO_KEY_TAB] = al_key_down(&keyboard_state, ALLEGRO_KEY_TAB);
	io.KeysDown[ALLEGRO_KEY_LEFT] = al_key_down(&keyboard_state, ALLEGRO_KEY_LEFT);
	io.KeysDown[ALLEGRO_KEY_RIGHT] = al_key_down(&keyboard_state, ALLEGRO_KEY_RIGHT);
	io.KeysDown[ALLEGRO_KEY_UP] = al_key_down(&keyboard_state, ALLEGRO_KEY_UP);
	io.KeysDown[ALLEGRO_KEY_DOWN] = al_key_down(&keyboard_state, ALLEGRO_KEY_DOWN);
	io.KeysDown[ALLEGRO_KEY_PGUP] = al_key_down(&keyboard_state, ALLEGRO_KEY_PGUP);
	io.KeysDown[ALLEGRO_KEY_PGDN] = al_key_down(&keyboard_state, ALLEGRO_KEY_PGDN);
	io.KeysDown[ALLEGRO_KEY_HOME] = al_key_down(&keyboard_state, ALLEGRO_KEY_HOME);
	io.KeysDown[ALLEGRO_KEY_END] = al_key_down(&keyboard_state, ALLEGRO_KEY_END);
	io.KeysDown[ALLEGRO_KEY_DELETE] = al_key_down(&keyboard_state, ALLEGRO_KEY_DELETE);
	io.KeysDown[ALLEGRO_KEY_BACKSPACE] = al_key_down(&keyboard_state, ALLEGRO_KEY_BACKSPACE);
	io.KeysDown[ALLEGRO_KEY_ENTER] = al_key_down(&keyboard_state, ALLEGRO_KEY_ENTER);
	io.KeysDown[ALLEGRO_KEY_ESCAPE] = al_key_down(&keyboard_state, ALLEGRO_KEY_ESCAPE);
	io.KeysDown[ALLEGRO_KEY_A] = al_key_down(&keyboard_state, ALLEGRO_KEY_A);
	io.KeysDown[ALLEGRO_KEY_C] = al_key_down(&keyboard_state, ALLEGRO_KEY_C);
	io.KeysDown[ALLEGRO_KEY_V] = al_key_down(&keyboard_state, ALLEGRO_KEY_V);
	io.KeysDown[ALLEGRO_KEY_X] = al_key_down(&keyboard_state, ALLEGRO_KEY_X);
	io.KeysDown[ALLEGRO_KEY_Y] = al_key_down(&keyboard_state, ALLEGRO_KEY_Y);
	io.KeysDown[ALLEGRO_KEY_Z] = al_key_down(&keyboard_state, ALLEGRO_KEY_Z);
	io.KeyCtrl = al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) || al_key_down(&keyboard_state, ALLEGRO_KEY_RCTRL);
	io.KeyShift = al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT) || al_key_down(&keyboard_state, ALLEGRO_KEY_RSHIFT);
	io.KeyAlt = al_key_down(&keyboard_state, ALLEGRO_KEY_ALT);

	ImGui::NewFrame();
}

void principle::GUIManager::stop()
{
	// Nada.
}

void principle::GUIManager::render()
{
	ImGui::Render();

	ImDrawData* data = ImGui::GetDrawData();
	for (int i = 0; i < data->CmdListsCount; ++i)
	{
		render(*(data->CmdLists[i]));
	}
}

void principle::GUIManager::render(ImDrawList& draw_list)
{
	if (draw_list.VtxBuffer.Size == 0)
	{
		return;
	}

	glViewport(0, 0, this->display->get_width(), this->display->get_height());
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);

	this->shader->use();

	auto projection = glm::ortho(
		0.0f, (float)this->display->get_width(),
		(float)this->display->get_height(), 0.0f,
		-1.0f, 1.0f);
	this->shader->set("principle_Projection", projection);
	this->shader->set("principle_Texture", 1);

	kompvter::Buffer vertex_buffer;
	vertex_buffer.allocate(
		draw_list.VtxBuffer.Size * sizeof(ImDrawVert));
	vertex_buffer.update(
		0,
		(const std::uint8_t*)draw_list.VtxBuffer.Data,
		vertex_buffer.get_data_size());
	this->vertex_buffer->set_buffer(vertex_buffer);

	kompvter::Buffer index_buffer;
	index_buffer.allocate(
		draw_list.IdxBuffer.Size * sizeof(ImDrawIdx));
	index_buffer.update(
		0,
		(const std::uint8_t*)draw_list.IdxBuffer.Data,
		index_buffer.get_data_size());
	this->index_buffer->set_buffer(index_buffer);

	this->mesh->enable_vertex_attrib(
		kvncer::Shader::vertex_input_position,
		this->vertex_buffer.get(),
		2, GL_FLOAT, GL_FALSE, offsetof(ImDrawVert, pos), sizeof(ImDrawVert));
	this->mesh->enable_vertex_attrib(
		kvncer::Shader::vertex_input_texture_coordinate,
		this->vertex_buffer.get(),
		2, GL_FLOAT, GL_FALSE, offsetof(ImDrawVert, uv), sizeof(ImDrawVert));
	this->mesh->enable_vertex_attrib(
		kvncer::Shader::vertex_input_color,
		this->vertex_buffer.get(),
		 4, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(ImDrawVert, col), sizeof(ImDrawVert));
	this->mesh->set_indices(this->index_buffer.get());

	std::size_t index_buffer_offset = 0;
	for (int j = 0; j < draw_list.CmdBuffer.Size; ++j)
	{
		auto& command = draw_list.CmdBuffer[j];
		if (command.UserCallback)
		{
			command.UserCallback(&draw_list, &command);
		}
		else
		{
			auto texture = (kvncer::Texture*)command.TextureId;
			if (texture != nullptr)
			{
				texture->bind(0);
			}
			else
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			glScissor(
				command.ClipRect.x,
				this->display->get_height() - command.ClipRect.w,
				command.ClipRect.z - command.ClipRect.x,
				command.ClipRect.w - command.ClipRect.y);

			auto index_size = sizeof(ImDrawIdx);
			auto stop = index_buffer_offset + command.ElemCount * index_size;
			if (index_size == 2)
			{
				mesh->draw(GL_UNSIGNED_SHORT, index_buffer_offset, stop);
			}
			else if (index_size == 4)
			{
				mesh->draw(GL_UNSIGNED_INT, index_buffer_offset, stop);
			}
			else
			{
				assert(false);
			}

			index_buffer_offset += command.ElemCount * index_size;
		}
	}
}

void principle::GUIManager::send_text_input(unsigned int c)
{
	auto& io = ImGui::GetIO();
	io.AddInputCharacter(c);
}
