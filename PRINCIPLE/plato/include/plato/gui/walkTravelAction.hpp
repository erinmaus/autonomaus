// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_GUI_WALK_LOCATION_RESOURCE_HPP
#define PLATO_GUI_WALK_LOCATION_RESOURCE_HPP

#include "plato/gui/brochureWidgetManager.hpp"
#include "plato/tools/toolbox.hpp"

namespace plato
{
	class WalkTravelActionView : public ActionView
	{
	public:
		WalkLocationActionView();
		~WalkLocationActionView() = default;

		void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			const twoflower::Action& resource) const override;
	};

	class WalkTravelActionEditor : public ActionEditor
	{
	public:
		WalkTravelActionEditor();
		~WalkTravelActionEditor() = default;

		void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			twoflower::Action& resource) const override;
	};
}

#endif
