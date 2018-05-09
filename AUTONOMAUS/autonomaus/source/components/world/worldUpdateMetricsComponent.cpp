// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/components/world/worldUpdateMetricsComponent.hpp"
#include "autonomaus/deps/imgui/imgui.h"

const std::size_t autonomaus::WorldUpdateMetricsComponent::HISTORY;

autonomaus::WorldUpdateMetricsComponent::WorldUpdateMetricsComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	this->second_updates.resize(HISTORY, 0.0f);
	this->minute_updates.resize(HISTORY, 0.0f);
}

void autonomaus::WorldUpdateMetricsComponent::show()
{
	this->visible = true;
}

void autonomaus::WorldUpdateMetricsComponent::reload()
{
	this->second_updates.clear();
	this->second_updates.resize(HISTORY, 0.0f);
	this->minute_updates.clear();
	this->minute_updates.resize(HISTORY, 0.0f);
}

void autonomaus::WorldUpdateMetricsComponent::update()
{
	auto& world = get_autonomaus().get_world_state();
	auto second = world->get_second_metric();
	auto minute = world->get_minute_metric();

	if (second.timestamp > this->last_second_update)
	{
		this->second_updates.insert(this->second_updates.begin(), second.median);
		this->second_updates.resize(HISTORY);
		this->last_second_update = second.timestamp;
	}

	if (minute.timestamp > this->last_minute_update)
	{
		this->minute_updates.insert(this->minute_updates.begin(), minute.median);
		this->second_updates.resize(HISTORY);
		this->last_minute_update = minute.timestamp;
	}

	if (this->visible)
	{
		draw();
	}
}

std::string autonomaus::WorldUpdateMetricsComponent::get_name() const
{
	return "World Update Metrics";
}

void autonomaus::WorldUpdateMetricsComponent::draw()
{
	auto& world = get_autonomaus().get_world_state();
	ImGui::Begin("World Update Metrics", &this->visible);
	{
		draw_metric("Second", world->get_second_metric());
		ImGui::PlotLines("Last Minute Medians", &this->second_updates[0], this->second_updates.size());
		draw_metric("Minute", world->get_minute_metric());
		ImGui::PlotLines("Last Hour Medians", &this->minute_updates[0], this->minute_updates.size());
		draw_metric("Hour", world->get_hour_metric());
	}
	ImGui::End();
}

void autonomaus::WorldUpdateMetricsComponent::draw_metric(
	const std::string& title, const World::Metric& metric)
{
	ImGui::BeginGroup();
	{
		ImGui::Text("%s", title.c_str());
		ImGui::BulletText("count: %d", metric.count);
		if (metric.count == 0)
		{
			ImGui::BulletText("median: (n/a)");
			ImGui::BulletText("min: (n/a)");
			ImGui::BulletText("max: (n/a)");
		}
		else
		{
			ImGui::BulletText("median: %.01f ms", metric.median);
			ImGui::BulletText("min: %.01f ms", metric.min);
			ImGui::BulletText("max: %.01f ms", metric.max);
		}
	}
	ImGui::EndGroup();

	ImGui::Separator();
	ImGui::Spacing();
}