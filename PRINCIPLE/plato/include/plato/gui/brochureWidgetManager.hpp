// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_GUI_BROCHURE_WIDGET_MANAGER_HPP
#define PLATO_GUI_BROCHURE_WIDGET_MANAGER_HPP

#include <functional>
#include <string>
#include <map>
#include "twoflower/brochure.hpp"
#include "twoflower/relationships/action.hpp"
#include "twoflower/relationships/resource.hpp"
#include "mapp/atlas/atlas.hpp"
#include "mapp/game/game.hpp"

namespace plato
{
	class BrochureWidgetManager;
	class Toolbox;

	class ResourceView
	{
	public:
		virtual void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			const twoflower::Resource& resource) const = 0;
	};

	class ActionView
	{
	public:
		virtual void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			const twoflower::Action& action) const = 0;
	};

	class ResourceEditor
	{
	public:
		virtual void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			twoflower::Resource& resource) const = 0;
	};

	class ActionEditor
	{
	public:
		virtual void update(
			BrochureWidgetManager& manager,
			twoflower::Brochure& brochure,
			twoflower::Action& action) const = 0;
	};

	class BrochureWidgetManager
	{
	public:
		BrochureWidgetManager(
			twoflower::Brochure& brochure,
			mapp::Atlas& atlas,
			mapp::Game& game,
			Toolbox& toolbox);
		~BrochureWidgetManager() = default;

		twoflower::Brochure& get_brochure() const;
		Toolbox& get_toolbox() const;

		mapp::Atlas& get_atlas();
		mapp::Game& get_game();

		void add(const std::string& resource_type, ResourceView* view);
		void add(const std::string& resource_type, ResourceEditor* editor);
		void view(const twoflower::Resource& resource, bool use_default = false);
		void edit(twoflower::Resource& resource, bool use_default = false);

		void add(const std::string& action_name, const std::string& action_type, ActionView* view);
		void add(const std::string& action_name, const std::string& action_type, ActionEditor* editor);
		void view(const twoflower::Action& action, bool use_default = false);
		void edit(twoflower::Action& action, bool use_default = false);

	private:
		twoflower::Brochure* brochure;
		Toolbox* toolbox;

		mapp::Atlas* atlas;
		mapp::Game* game;

		void view_fallback(const twoflower::Resource& resource);
		void view_fallback(const twoflower::Action& action);
		void edit_fallback(twoflower::Resource& resource);
		void edit_fallback(twoflower::Action& action);

		std::map<std::string, std::unique_ptr<ResourceView>> resource_views;
		std::map<std::string, std::unique_ptr<ResourceEditor>> resource_editors;

		std::map<std::pair<std::string, std::string>, std::unique_ptr<ActionView>> action_views;
		std::map<std::pair<std::string, std::string>, std::unique_ptr<ActionEditor>> action_editors;
	};
}

#endif
