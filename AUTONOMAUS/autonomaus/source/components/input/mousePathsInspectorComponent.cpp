// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/input/inputManager.hpp"
#include "autonomaus/graphics/guiOverlay.hpp"
#include "autonomaus/components/input/mousePathsInspectorComponent.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "eiznebahn/input/inputQueue.hpp"
#include "eiznebahn/mouse/mouseMovementProvider.hpp"

autonomaus::MousePathsInspectorComponent::MousePathsInspectorComponent(
	Autonomaus& autonomaus) : Component(autonomaus)
{
	// Nothing.
}

void autonomaus::MousePathsInspectorComponent::show()
{
	this->visible = true;
}

void autonomaus::MousePathsInspectorComponent::reload()
{
	// Nothing.
}

std::string autonomaus::MousePathsInspectorComponent::get_name() const
{
	return "Mouse Paths Inspector";
}

void autonomaus::MousePathsInspectorComponent::update()
{
	if (this->visible)
	{
		draw();
	}
}

void autonomaus::MousePathsInspectorComponent::draw()
{
	ImGui::Begin("Mouse path Inspector", &this->visible);
	{
		bool value_updated = false;
		ImGui::BeginGroup();
		{
			ImGui::Text("Property:");

			ImGui::RadioButton("Length", &this->property, property_length);
			ImGui::SameLine();
			ImGui::RadioButton("Direction", &this->property, property_direction);
			ImGui::SameLine();
			ImGui::RadioButton("Drift", &this->property, property_drift);
			ImGui::SameLine();
			ImGui::RadioButton("Duration", &this->property, property_duration);

			if (property == property_direction)
			{
				if (ImGui::SliderAngle("Value", &this->current_value, 0.0f, 360.f))
				{
					value_updated = true;
				}

				ImGui::SameLine();
				if (ImGui::SliderAngle("Threshold", &this->current_threshold, 0.0f, 360.0f))
				{
					value_updated = true;
				}
			}
			else
			{
				if (ImGui::InputFloat("Value", &this->current_value, 0.0f, 360.f))
				{
					value_updated = true;
				}

				ImGui::SameLine();
				if (ImGui::InputFloat("Threshold", &this->current_threshold, 0.0f, 360.0f))
				{
					value_updated = true;
				}
			}

			if (ImGui::Checkbox("Restrict to Threshold", &this->limit_threshold))
			{
				value_updated = true;
			}

			if (ImGui::InputInt("Limit", &this->limit))
			{
				if (this->limit < 0)
				{
					this->limit = 0;
				}

				if (this->limit >= 100)
				{
					this->limit = 99;
				}
			}
		}
		ImGui::EndGroup();

		if (value_updated || this->dirty)
		{
			update_filter();
		}

		ImGui::Separator();
		ImGui::Spacing();

		int count = std::min(this->limit, (int)this->filter.count());
		for (int i = 0; i < count; ++i)
		{
			ImGui::PushID(i);

			auto& path = this->filter.get(i);
			ImGui::Text("path % 2d:", i);
			ImGui::SameLine();
			if (ImGui::SmallButton("Preview"))
			{
				preview_path(path);
			}

			ImGui::SameLine();
			if (ImGui::SmallButton("Replay"))
			{
				preview_path(path);
				play_path(path);
			}

			ImGui::Bullet();
			ImGui::Value("Length (pixels)", path.get_length());
			ImGui::Bullet();
			ImGui::Value("Direction (degrees)", (int)(path.get_direction() * (180.0f / M_PI)));
			ImGui::Bullet();
			ImGui::Value("Drift (pixels)", path.get_drift());
			ImGui::Bullet();
			ImGui::Value("Drift:Length ratio", path.get_drift() / path.get_length());
			ImGui::Bullet();
			ImGui::Value("Duration (ms)", path.get_duration());

			ImGui::PopID();
		}
	}
	ImGui::End();
}

void autonomaus::MousePathsInspectorComponent::update_filter()
{
	this->filter.clear();

	auto& paths = get_autonomaus().get_input_manager().get_paths();
	this->filter.collect(paths.begin(), paths.end());
	this->filter.start(0);

	float threshold;
	if (this->limit_threshold)
	{
		threshold = this->current_threshold;
	}
	else
	{
		threshold = HUGE_VALF;
	}

	std::size_t count;
	switch (this->property)
	{
		case property_length:
			count = this->filter.length(this->current_value, threshold);
			break;
		case property_direction:
			count = this->filter.direction(this->current_value, threshold);
			break;
		case property_drift:
			count = this->filter.drift(this->current_value, threshold);
			break;
		case property_duration:
			this->filter.duration(this->current_value);
			count = paths.size();
			break;
		default:
			count = paths.size();
			break;
	}

	this->filter.select(count);
	this->filter.stop();
}

void autonomaus::MousePathsInspectorComponent::preview_path(
	const eiznebahn::MousePath& path)
{
	if (path.count() < 2)
	{
		return;
	}

	auto& viewport = get_autonomaus().get_draw_state().get_gui_viewport();

	glm::vec2 min(HUGE_VALF), max(-HUGE_VALF);
	std::vector<glm::vec2> points;
	std::vector<glm::vec2> shadow;
	for (auto point: path)
	{
		float x = std::get<eiznebahn::MousePath::X>(point);
		float y = -std::get<eiznebahn::MousePath::Y>(point);

		points.emplace_back(x, y);
		shadow.emplace_back(x + 2, y + 2);

		min = glm::min(min, points.back());
		max = glm::max(max, points.back());
	}

	glm::vec2 size = (max - min);
	glm::vec2 center;
	center.x = viewport.z / 2.0f - size.x / 2.0f;
	center.y = viewport.w / 2.0f - size.y / 2.0f;

	for (std::size_t i = 0; i < path.count(); ++i)
	{
		points[i] += center;
		points[i] -= min;
		points[i].y = viewport.w - points[i].y;
		shadow[i] += center;
		shadow[i] -= min;
		shadow[i].y = viewport.w - shadow[i].y;
	}

	glm::vec2 line[] =
	{
		points.front(),
		points.back()
	};

	auto& overlay = get_autonomaus().get_gui_draw_hook().after();
	GUIOverlay::Tag tag;
	overlay.start(tag, true, 4.0f);
	overlay.draw_path(line, 2, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 4.0f);
	overlay.draw_path(line, 2, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
	overlay.draw_path(&shadow[0], shadow.size(), glm::vec4(0.0f, 0.0f, 0.0f, 0.5f), 4.0f, false);
	overlay.draw_path(&points[0], points.size(), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 4.0f, false);
	overlay.stop();

	this->dirty = false;
}

void autonomaus::MousePathsInspectorComponent::play_path(
	const eiznebahn::MousePath& path)
{
	auto transformed_path = path;

	glm::vec2 min(HUGE_VALF), max(-HUGE_VALF);
	for (auto point: path)
	{
		float x = std::get<eiznebahn::MousePath::X>(point);
		float y = -std::get<eiznebahn::MousePath::Y>(point);

		glm::vec2 p(x, y);
		min = glm::min(min, p);
		max = glm::max(max, p);
	}

	auto& viewport = get_autonomaus().get_draw_state().get_gui_viewport();
	glm::vec2 center;
	glm::vec2 size = (max - min);
	center.x = viewport.z / 2.0f - size.x / 2.0f - min.x;
	center.y = viewport.w / 2.0f + size.y / 2.0f + min.y;

	transformed_path.translate(center.x, center.y);

	auto& queue = get_autonomaus().get_input_manager().get_queue();
	queue.start(eiznebahn::InputQueue::channel_mouse);
	queue.push<eiznebahn::MouseMovementProvider>(transformed_path);
	queue.stop();
}
