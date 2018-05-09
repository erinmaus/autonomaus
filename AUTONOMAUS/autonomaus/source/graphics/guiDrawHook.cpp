// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "autonomaus/core/time.hpp"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "eiznebahn/keyboard/keySequence.hpp"

static const char vertex_shader[] =
	"#version 140\n"
	"uniform mat4 maus_Projection;\n"
	"in vec3 vert_Position;\n"
	"in vec2 vert_Texture;\n"
	"in vec4 vert_Color;\n"
	"out vec2 frag_Texture;\n"
	"out vec4 frag_Color;\n"
	"void main()\n"
	"{\n"
	"	gl_Position =\n"
	"		vec4(vert_Position, 1.0) *\n"
	"		maus_Projection;\n"
	"	frag_Texture = vert_Texture;\n"
	"	frag_Color = vert_Color;\n"
	"}\n";
static const char fragment_shader[] =
	"#version 140\n"
	"uniform sampler2D maus_Texture;\n"
	"in vec2 frag_Texture;\n"
	"in vec4 frag_Color;\n"
	"out vec4 out_Color;\n"
	"void main()\n"
	"{\n"
	"	vec4 sample = frag_Color * texture(maus_Texture, frag_Texture);\n"
	"	sample *= frag_Color;\n"
	"	out_Color = vec4(sample.rgb * sample.a, sample.a);\n"
	"}\n";

autonomaus::GUIDrawHook::GUIDrawHook(
	touchstone::ImmediateDispatch& dispatch,
	DrawState& draw_state) :
		DrawHook(dispatch, draw_state)
{
	this->previous_time = get_current_time();
	before().set_draw_list(&before_draw_list);
	after().set_draw_list(&after_draw_list);
}

void autonomaus::GUIDrawHook::before_call(
	const touchstone::Context& context,
	std::uint32_t token)
{
	if (!is_drawing_to_back_buffer(context))
	{
		return;
	}

	if (token == touchstone::call::DrawArrays::token)
	{
		if (this->is_first_present)
		{
			auto& state = context.get_state();
			glm::ivec4 viewport;
			viewport.x = state.get_viewport_x();
			viewport.y = state.get_viewport_y();
			viewport.z = state.get_viewport_width();
			viewport.w = state.get_viewport_height();

			get_draw_state().set_world_viewport(viewport);
			this->is_first_present = false;
			this->presented_world = true;
		}
	}

	if (token != touchstone::call::DrawRangeElements::token)
	{
		return;
	}

	if (this->is_first_draw && this->presented_world)
	{
		this->presented_world = false;
		this->is_first_draw = false;
		update_viewport(context);
		draw_before();
	}
}

void autonomaus::GUIDrawHook::after_call(
	const touchstone::Context& context,
	const touchstone::Call* call)
{
	// Nothing.
}

void autonomaus::GUIDrawHook::invalidate()
{
	this->program.reset();
	this->font.reset();
	this->mesh.reset();

	ImGui::Shutdown();

	this->is_valid = false;
	this->can_draw_gui = false;
	this->is_viewport_updated = false;
}

void autonomaus::GUIDrawHook::load()
{
	this->program = std::make_unique<Program>(
		get_renderer(),
		vertex_shader, fragment_shader);
	this->mesh = std::make_unique<Mesh>(get_renderer());

	auto& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	this->font = std::make_unique<Texture>(get_renderer());
	this->font->set_pixels(width, height, pixels);
	io.Fonts->TexID = this->font.get();

	io.KeyMap[ImGuiKey_Tab] = eiznebahn::KeySequence::key_code_tab;
	io.KeyMap[ImGuiKey_LeftArrow] = eiznebahn::KeySequence::key_code_left;
	io.KeyMap[ImGuiKey_RightArrow] = eiznebahn::KeySequence::key_code_right;
	io.KeyMap[ImGuiKey_UpArrow] = eiznebahn::KeySequence::key_code_up;
	io.KeyMap[ImGuiKey_DownArrow] = eiznebahn::KeySequence::key_code_down;
	io.KeyMap[ImGuiKey_PageUp] = eiznebahn::KeySequence::key_code_page_up;
	io.KeyMap[ImGuiKey_PageDown] = eiznebahn::KeySequence::key_code_page_down;
	io.KeyMap[ImGuiKey_Home] = eiznebahn::KeySequence::key_code_home;
	io.KeyMap[ImGuiKey_End] = eiznebahn::KeySequence::key_code_end;
	io.KeyMap[ImGuiKey_Delete] = eiznebahn::KeySequence::key_code_delete;
	io.KeyMap[ImGuiKey_Backspace] = eiznebahn::KeySequence::key_code_backspace;
	io.KeyMap[ImGuiKey_Enter] = eiznebahn::KeySequence::key_code_return;
	io.KeyMap[ImGuiKey_Escape] = eiznebahn::KeySequence::key_code_escape;
	io.KeyMap[ImGuiKey_A] = eiznebahn::KeySequence::key_code_a;
	io.KeyMap[ImGuiKey_C] = eiznebahn::KeySequence::key_code_c;
	io.KeyMap[ImGuiKey_V] = eiznebahn::KeySequence::key_code_v;
	io.KeyMap[ImGuiKey_X] = eiznebahn::KeySequence::key_code_x;
	io.KeyMap[ImGuiKey_Y] = eiznebahn::KeySequence::key_code_y;
	io.KeyMap[ImGuiKey_Z] = eiznebahn::KeySequence::key_code_z;

	// From https://github.com/ocornut/imgui/issues/707#issuecomment-254424199
	static const ImVec4 color_for_text = ImVec4(253.f / 240.f, 240.f / 255.f, 213.f / 255.f, 1.0f);
	static const ImVec4 color_for_head = ImVec4(52.f / 255.f, 63.f / 255.f, 66.f / 255.f, 1.0f);
	static const ImVec4 color_for_body = ImVec4(75.f / 255.f, 117.f / 255.f, 127.f / 255.f, 1.0f);
	static const ImVec4 color_for_area = ImVec4(95.f / 255.f, 132.f / 255.f, 140.f / 255.f, 1.0f);
	static const ImVec4 color_for_pops = ImVec4(74.f / 255.f, 96.f / 255.f, 102.f / 255.f, 1.0f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 1.00f );
	style.Colors[ImGuiCol_TextDisabled] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.58f );
	style.Colors[ImGuiCol_WindowBg] = ImVec4( color_for_body.x, color_for_body.y, color_for_body.z, 0.95f );
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.58f );
	style.Colors[ImGuiCol_Border] = ImVec4( color_for_body.x, color_for_body.y, color_for_body.z, 0.00f );
	style.Colors[ImGuiCol_BorderShadow] = ImVec4( color_for_body.x, color_for_body.y, color_for_body.z, 0.00f );
	style.Colors[ImGuiCol_FrameBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 1.00f );
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_TitleBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 1.00f );
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.75f );
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.47f );
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 1.00f );
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.21f );
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_ComboBg] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 1.00f );
	style.Colors[ImGuiCol_CheckMark] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.80f );
	style.Colors[ImGuiCol_SliderGrab] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.50f );
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_Button] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.50f );
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.86f );
	style.Colors[ImGuiCol_ButtonActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_Header] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.76f );
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.86f );
	style.Colors[ImGuiCol_HeaderActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_Column] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.32f );
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
	style.Colors[ImGuiCol_ColumnActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.15f );
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.78f );
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_CloseButton] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.16f );
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.39f );
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 1.00f );
	style.Colors[ImGuiCol_PlotLines] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.63f );
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4( color_for_text.x, color_for_text.y, color_for_text.z, 0.63f );
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 1.00f );
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( color_for_head.x, color_for_head.y, color_for_head.z, 0.43f );
	style.Colors[ImGuiCol_PopupBg] = ImVec4( color_for_pops.x, color_for_pops.y, color_for_pops.z, 0.92f );
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4( color_for_area.x, color_for_area.y, color_for_area.z, 0.73f );

	this->is_first_draw = true;
	this->is_first_present = true;
	this->is_valid = true;
}

void autonomaus::GUIDrawHook::on_flip(const touchstone::Context& context)
{
	if (!this->is_valid)
	{
		return;
	}

	update_viewport(context);
	draw_after();

	this->is_first_draw = true;
	this->is_first_present = true;
}

void autonomaus::GUIDrawHook::update_input(
	const eiznebahn::InputState& input_state)
{
	if (!this->is_viewport_updated)
	{
		return;
	}

	using namespace eiznebahn;
	auto& io = ImGui::GetIO();
	io.KeysDown[KeySequence::key_code_tab] =
		input_state.is_key_pressed(KeySequence::key_code_tab);
	io.KeysDown[KeySequence::key_code_left] =
		input_state.is_key_pressed(KeySequence::key_code_left);
	io.KeysDown[KeySequence::key_code_right] =
		input_state.is_key_pressed(KeySequence::key_code_right);
	io.KeysDown[KeySequence::key_code_up] =
		input_state.is_key_pressed(KeySequence::key_code_up);
	io.KeysDown[KeySequence::key_code_down] =
		input_state.is_key_pressed(KeySequence::key_code_down);
	io.KeysDown[KeySequence::key_code_page_up] =
		input_state.is_key_pressed(KeySequence::key_code_page_up);
	io.KeysDown[KeySequence::key_code_page_down] =
		input_state.is_key_pressed(KeySequence::key_code_page_down);
	io.KeysDown[KeySequence::key_code_home] =
		input_state.is_key_pressed(KeySequence::key_code_home);
	io.KeysDown[KeySequence::key_code_end] =
		input_state.is_key_pressed(KeySequence::key_code_end);
	io.KeysDown[KeySequence::key_code_delete] =
		input_state.is_key_pressed(KeySequence::key_code_delete);
	io.KeysDown[KeySequence::key_code_backspace] =
		input_state.is_key_pressed(KeySequence::key_code_backspace);
	io.KeysDown[KeySequence::key_code_return] =
		input_state.is_key_pressed(KeySequence::key_code_return);
	io.KeysDown[KeySequence::key_code_escape] =
		input_state.is_key_pressed(KeySequence::key_code_escape);
	io.KeysDown[KeySequence::key_code_a] =
		input_state.is_key_pressed(KeySequence::key_code_a);
	io.KeysDown[KeySequence::key_code_c] =
		input_state.is_key_pressed(KeySequence::key_code_c);
	io.KeysDown[KeySequence::key_code_v] =
		input_state.is_key_pressed(KeySequence::key_code_v);
	io.KeysDown[KeySequence::key_code_x] =
		input_state.is_key_pressed(KeySequence::key_code_x);
	io.KeysDown[KeySequence::key_code_y] =
		input_state.is_key_pressed(KeySequence::key_code_y);
	io.KeysDown[KeySequence::key_code_z] =
		input_state.is_key_pressed(KeySequence::key_code_z);
	io.KeyCtrl =
		input_state.is_key_pressed(KeySequence::key_code_left_ctrl) ||
		input_state.is_key_pressed(KeySequence::key_code_right_ctrl);
	io.KeyShift =
		input_state.is_key_pressed(KeySequence::key_code_left_shift) ||
		input_state.is_key_pressed(KeySequence::key_code_right_shift);
	io.KeyAlt =
		input_state.is_key_pressed(KeySequence::key_code_left_alt) ||
		input_state.is_key_pressed(KeySequence::key_code_right_alt);

	io.MousePos = ImVec2(input_state.get_mouse_x(), input_state.get_mouse_y());
	io.MouseDown[0] = input_state.is_mouse_button_pressed(MouseButton::button_left);
	io.MouseDown[1] = input_state.is_mouse_button_pressed(MouseButton::button_right);
	io.MouseDown[2] = input_state.is_mouse_button_pressed(MouseButton::button_middle);
	io.MouseWheel = input_state.get_mouse_scroll() - this->previous_mouse_scroll;
	this->previous_mouse_scroll = input_state.get_mouse_scroll();

	if (io.WantTextInput)
	{
		auto text = input_state.get_text_input();
		for (auto c: text)
		{
			io.AddInputCharacter(c);
		}
	}

	float current_time = get_current_time();
	io.DeltaTime = current_time - this->previous_time;
	this->previous_time = current_time;

	ImGui::NewFrame();
	this->can_draw_gui = true;
}

bool autonomaus::GUIDrawHook::wants_mouse_captured() const
{
	if (this->can_draw_gui)
	{
		return ImGui::GetIO().WantCaptureMouse;
	}

	return false;
}

bool autonomaus::GUIDrawHook::wants_keyboard_captured() const
{
	if (this->can_draw_gui)
	{
		return ImGui::GetIO().WantCaptureKeyboard;
	}

	return false;
}

bool autonomaus::GUIDrawHook::is_ready() const
{
	return this->can_draw_gui && this->is_viewport_updated;
}

void autonomaus::GUIDrawHook::before_draw_overlay(ImDrawList& list)
{
	list.Clear();

	list.PushClipRect(ImVec2(0, 0), ImVec2(this->viewport_width, this->viewport_height));
	list.PushTextureID(ImGui::GetIO().Fonts->Fonts[0]->ContainerAtlas->TexID);
}

void autonomaus::GUIDrawHook::after_draw_overlay(ImDrawList& list)
{
	list.PopTextureID();

	prepare_imgui_draw_state();
	draw_imgui_draw_list(&list);
}

void autonomaus::GUIDrawHook::draw_before()
{
	if (!this->can_draw_gui)
	{
		return;
	}

	get_renderer().begin();
	{
		before_draw_overlay(before_draw_list);
		before().draw(get_current_time());
		after_draw_overlay(before_draw_list);
	}
	get_renderer().end();
}

void autonomaus::GUIDrawHook::draw_after()
{
	if (!this->can_draw_gui)
	{
		return;
	}

	get_renderer().begin();
	{
		before_draw_overlay(after_draw_list);
		after().draw(get_current_time());
		after_draw_overlay(after_draw_list);

		draw_imgui();
	}
	get_renderer().end();
}

void autonomaus::GUIDrawHook::draw_imgui()
{
	prepare_imgui_draw_state();

	auto& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(this->viewport_width, this->viewport_height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	ImGui::Render();
	ImDrawData* data = ImGui::GetDrawData();
	for (int i = 0; i < data->CmdListsCount; ++i)
	{
		draw_imgui_draw_list(data->CmdLists[i]);
	}
}

void autonomaus::GUIDrawHook::prepare_imgui_draw_state()
{
	get_renderer().disable_depth_testing();
	get_renderer().set_blend_mode(Renderer::blend_alpha);

	this->program->use();
	auto projection = glm::ortho(
		0.0f, (float)this->viewport_width,
		(float)this->viewport_height, 0.0f,
		-1.0f, 1.0f);
	this->program->set("maus_Projection", projection);
	this->program->set("maus_Texture", (int)get_renderer().get_active_unit());
}

void autonomaus::GUIDrawHook::draw_imgui_draw_list(ImDrawList* list)
{
	std::vector<glm::vec3> vertex_positions;
	std::vector<glm::vec2> vertex_textures;
	std::vector<glm::vec4> vertex_colors;
	vertex_positions.resize(list->VtxBuffer.Size);
	vertex_textures.resize(list->VtxBuffer.Size);
	vertex_colors.resize(list->VtxBuffer.Size);
	for (int j = 0; j < list->VtxBuffer.Size; ++j)
	{
		vertex_positions[j].x = list->VtxBuffer.Data[j].pos.x;
		vertex_positions[j].y = list->VtxBuffer.Data[j].pos.y;
		vertex_positions[j].z = 0;

		vertex_textures[j].s = list->VtxBuffer.Data[j].uv.x;
		vertex_textures[j].t = list->VtxBuffer.Data[j].uv.y;

		auto color = (const glm::u8vec4*)&list->VtxBuffer.Data[j].col;
		vertex_colors[j].r = color->r / 255.0f;
		vertex_colors[j].g = color->g / 255.0f;
		vertex_colors[j].b = color->b / 255.0f;
		vertex_colors[j].a = color->a / 255.0f;
	}

	this->mesh->set_num_vertices(list->VtxBuffer.Size);
	this->mesh->set_positions(&vertex_positions[0]);
	this->mesh->set_textures(&vertex_textures[0]);
	this->mesh->set_colors(&vertex_colors[0]);
	this->mesh->set_indices(
		list->IdxBuffer.Data,
		list->IdxBuffer.Size);

	std::size_t index_buffer_offset = 0;
	for (int j = 0; j < list->CmdBuffer.Size; ++j)
	{
		auto& command = list->CmdBuffer[j];
		if (command.UserCallback != nullptr)
		{
			command.UserCallback(list, &command);
			continue;
		}

		auto texture = (Texture*)command.TextureId;
		if (texture != nullptr)
		{
			texture->bind();
		}

		get_dispatch().Enable(GL_SCISSOR_TEST);
		get_renderer().set_scissor(
			command.ClipRect.x,
			this->viewport_height - command.ClipRect.w,
			command.ClipRect.z - command.ClipRect.x,
			command.ClipRect.w - command.ClipRect.y);
		this->mesh->draw(
			Mesh::primitive_triangles,
			index_buffer_offset, command.ElemCount);
		index_buffer_offset += command.ElemCount;
	}
}

void autonomaus::GUIDrawHook::update_viewport(
	const touchstone::Context& context)
{
	auto& state = context.get_state();
	this->viewport_x = state.get_viewport_x();
	this->viewport_y = state.get_viewport_y();
	this->viewport_width = state.get_viewport_width();
	this->viewport_height = state.get_viewport_height();

	glm::ivec4 viewport;
	viewport.x = state.get_viewport_x();
	viewport.y = state.get_viewport_y();
	viewport.z = state.get_viewport_width();
	viewport.w = state.get_viewport_height();
	get_draw_state().set_gui_viewport(viewport);

	auto& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(this->viewport_width, this->viewport_height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	this->is_viewport_updated = true;
}

bool autonomaus::GUIDrawHook::is_drawing_to_back_buffer(
	const touchstone::Context& context)
{
	auto binding_manager = context.get_binding_manager<touchstone::FrameBufferResource>();
	if (binding_manager.is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		return false;
	}

	return true;
}
