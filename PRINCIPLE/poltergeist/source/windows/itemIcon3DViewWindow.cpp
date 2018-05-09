// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "kvre/model/model.hpp"
#include "poltergeist/application.hpp"
#include "principle/deps/imgui/imgui.h"

poltergeist::Application::ItemIcon3DViewWindow::ItemIcon3DViewWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

void poltergeist::Application::ItemIcon3DViewWindow::open(
	const principle::ItemIcon& icon)
{
	this->icon = &icon;
	this->thumbnail.reset();
	principle::Window::open();
}

void poltergeist::Application::ItemIcon3DViewWindow::close()
{
	this->icon = nullptr;
	this->thumbnail.reset();
	principle::Window::close();
}

std::string poltergeist::Application::ItemIcon3DViewWindow::get_title() const
{
	return "Item Icon 3D View";
}

void poltergeist::Application::ItemIcon3DViewWindow::update_gui()
{
	show_thumbnail();
	ImGui::Spacing();

	ImGui::Button("View Hash");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::BeginChildFrame(0, ImVec2(256, 160));
		ImGui::TextWrapped("%s", this->icon->get_silhouette().to_string().c_str());
		ImGui::EndChildFrame();
		ImGui::EndTooltip();
	}

	ImGui::SameLine();
	if (ImGui::Button("Copy Hash"))
	{
		ImGui::SetClipboardText(this->icon->get_silhouette().to_string().c_str());
	}

	if (ImGui::CollapsingHeader("Background Color"))
	{
		ImGui::ColorEdit4("Value", glm::value_ptr(this->background_color));
	}

	if (ImGui::CollapsingHeader("Lighting"))
	{
		ImGui::DragFloat3("Position", glm::value_ptr(this->light_position), 16.0f);
		ImGui::ColorEdit3("Color", glm::value_ptr(this->light_color));
		ImGui::DragFloat("Intensity", &this->light_intensity, 0.125f, 0.0f, 4.0f);
		ImGui::DragFloat("Ambience", &this->light_ambience, 0.125f, 0.0f, 1.0f);
	}
}

kvncer::Texture* poltergeist::Application::ItemIcon3DViewWindow::update_thumbnail()
{
	if (!this->thumbnail || this->thumbnail->get_width() != ImGui::GetWindowSize().x)
	{
		int width = ImGui::GetWindowSize().x;
		int height = width * (9.0f / 16.0f);
		this->thumbnail = std::make_unique<principle::Thumbnail>(width, height);
		this->thumbnail->load_shader_from_file(
			"model.vert.glsl", "untextured_lit_model.frag.glsl");

		if (this->icon != nullptr)
		{
			for (std::size_t i = 0; i < this->icon->get_num_draws(); ++i)
			{
				auto& draw = this->icon->get_draw(i);
				kvre::Model model(draw.get_vertex_attributes());
				this->thumbnail->add_model(
					&model,
					glm::mat4(1.0f),
					draw.get_vertex_index_mapper(),
					draw.get_draw_parameters());
			}
		}
	}
	this->thumbnail->set_background_color(this->background_color);

	glm::mat4 x = glm::rotate(glm::mat4(1.0f), this->x_rotation, glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 y = glm::rotate(glm::mat4(1.0f), this->y_rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(this->scale));

	auto shader = this->thumbnail->get_shader();
	shader->use();
	shader->set("principle_LightPosition", this->light_position);
	shader->set("principle_LightColor", this->light_color);
	shader->set("principle_LightIntensity", this->light_intensity);
	shader->set("principle_Ambience", this->light_ambience);

	return this->thumbnail->draw(s * x * y);
}

void poltergeist::Application::ItemIcon3DViewWindow::show_thumbnail()
{
	auto thumbnail_image = update_thumbnail();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChildFrame(0, ImVec2(thumbnail_image->get_width(), thumbnail_image->get_height()));
	ImGui::Image(thumbnail_image, ImVec2(thumbnail_image->get_width(), thumbnail_image->get_height()));
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseDragging(0))
		{
			this->x_rotation += ImGui::GetMouseDragDelta(0).y / 64.0f;
			this->y_rotation += ImGui::GetMouseDragDelta(0).x / 64.0f;
			ImGui::ResetMouseDragDelta(0);
		}

		if (ImGui::IsMouseDragging(1))
		{
			this->scale += ImGui::GetMouseDragDelta(1).y / 64.0f;
			ImGui::ResetMouseDragDelta(1);
		}
	}
	ImGui::EndChildFrame();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}
