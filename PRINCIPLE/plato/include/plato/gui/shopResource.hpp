// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_GUI_SHOP_RESOURCE_HPP
#define PLATO_GUI_SHOP_RESOURCE_HPP

#include "plato/gui/brochureWidgetManager.hpp"
#include "plato/tools/toolbox.hpp"

namespace plato
{
	class ShopResourceView : public ResourceView
	{
	public:
		ShopResourceView() = default;
		~ShopResourceView() = default;

		void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			const twoflower::Resource& resource) const override;
	};

	class ShopResourceEditor : public ResourceEditor
	{
	public:
		ShopResourceEditor() = default;
		~ShopResourceEditor() = default;

		void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			twoflower::Resource& resource) const override;
	};
}

#endif
