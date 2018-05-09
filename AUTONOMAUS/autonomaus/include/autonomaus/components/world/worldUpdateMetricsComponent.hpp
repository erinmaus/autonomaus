// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_WORLD_WORLD_UPDATE_METRICS_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_WORLD_WORLD_UPDATE_METRICS_COMPONENT_HPP

#include <vector>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "autonomaus/components/component.hpp"

namespace autonomaus
{
	class WorldUpdateMetricsComponent : public Component
	{
	public:
		explicit WorldUpdateMetricsComponent(Autonomaus& autonomaus);
		~WorldUpdateMetricsComponent() = default;

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		bool visible = false;
		void draw();
		void draw_metric(const std::string& title, const World::Metric& metric);

		static const std::size_t HISTORY = 60;
		std::vector<float> second_updates;
		float last_second_update = 0.0f;
		std::vector<float> minute_updates;
		float last_minute_update = 0.0f;
	};
}

#endif
