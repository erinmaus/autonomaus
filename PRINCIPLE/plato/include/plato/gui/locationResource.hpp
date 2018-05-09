// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_GUI_LOCATION_RESOURCE_HPP
#define PLATO_GUI_LOCATION_RESOURCE_HPP

#include "plato/gui/brochureWidgetManager.hpp"
#include "plato/tools/toolbox.hpp"

namespace plato
{
	class LocationResourceView : public ResourceView
	{
	public:
		void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			const twoflower::Resource& resource) const override;
	};

	class LocationResourceEditor : public ResourceEditor
	{
	public:
		void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			twoflower::Resource& resource) const override;
	};
}

#endif
