// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_MOUSE_PATHS_INSPECTOR_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_MOUSE_PATHS_INSPECTOR_COMPONENT_HPP

#include <functional>
#include <vector>
#include "autonomaus/components/component.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/input/mousePathFilter.hpp"

namespace autonomaus
{
	class MousePathsInspectorComponent : public Component
	{
	public:
		explicit MousePathsInspectorComponent(Autonomaus& autonomaus);
		~MousePathsInspectorComponent() = default;

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		bool visible = false;
		void draw();

		MousePathFilter filter;
		bool dirty = true;
		float current_value = 200.0f, current_threshold = 0.0f;
		bool limit_threshold = false;
		int limit = 10;
		enum
		{
			property_length = 0,
			property_direction,
			property_drift,
			property_duration
		};
		int property = property_length;
		void update_filter();

		void preview_path(const eiznebahn::MousePath& path);
		void play_path(const eiznebahn::MousePath& path);
	};
}

#endif
