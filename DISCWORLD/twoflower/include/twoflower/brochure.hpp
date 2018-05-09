// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_BROCHURE_HPP
#define TWOFLOWER_BROCHURE_HPP

#include <cstdint>
#include <memory>
#include <unordered_set>
#include <vector>
#include "twoflower/relationships/action.hpp"
#include "twoflower/relationships/requirement.hpp"
#include "twoflower/relationships/resource.hpp"

namespace twoflower
{
	class Action;
	class Plan;
	class Requirement;
	class Resource;

	class Brochure
	{
	public:
		class Builder;

		explicit Brochure(const std::string& filename = "");
		~Brochure();

		Builder builder();

		bool has_resource_type(const std::string& name) const;
		bool has_action_definition(const std::string& type, const std::string& name) const;

		bool get_userdata(
			const Resource& resource,
			const std::string& field,
			int& value) const;
		bool get_userdata(
			const Resource& resource,
			const std::string& field,
			float& value) const;
		bool get_userdata(
			const Resource& resource,
			const std::string& field,
			std::string& value) const;
		bool get_userdata(
			const Resource& resource,
			const std::string& field,
			std::vector<std::uint8_t>& value) const;
		bool has_userdata(
			const Resource& resource,
			const std::string& field) const;

		bool has_tag(const Resource& resource, const std::string& value) const;

		typedef std::unordered_set<std::string> Tags;
		Tags tags(const Resource& resource) const;

		Resource resource(const Action& action) const;

		class Resources;
		Resources resources() const;

		class Actions;
		Actions actions() const;
		Actions actions(const Resource& resource) const;

		class Requirements;
		Requirements requirements(const Action& action) const;

		bool plan(
			Plan& result,
			const Resource& resource,
			const Action& action,
			int quantity = 1) const;

	private:
		class Database;
		class Statement;
		class Table;

		void make_tables();
		void create_table(Table& table);

		std::shared_ptr<Database> database;
	};

	class Brochure::Builder
	{
	public:
		Builder(Brochure& brochure);

		void add_resource_type(const std::string& name);
		void remove_resource_type(const std::string& name);

		Resource add_resource(const Resource& resource);
		void update_resource(const Resource& resource);
		void remove_resource(const Resource& resource);

		void add_tag(const Resource& resource, const std::string& tag);
		void remove_tag(const Resource& resource, const std::string& tag);

		void set_userdata(
			const Resource& resource,
			const std::string& field,
			int value);
		void set_userdata(
			const Resource& resource,
			const std::string& field,
			float value);
		void set_userdata(
			const Resource& resource,
			const std::string& field,
			const std::string& value);
		void set_userdata(
			const Resource& resource,
			const std::string& field,
			const std::vector<std::uint8_t>& value);
		void unset_userdata(const Resource& resource, const std::string& key);

		void add_action_definition(const Action& action);
		void update_action(const Action& action);
		void remove_action(const Action& action);

		void update_requirement(const Requirement& requirement);
		void remove_requirement(const Requirement& requirement);

		Action connect(const Action& action, const Resource& resource);
		Requirement connect(
			const Requirement& requirement,
			const Action& action,
			const Resource& resource);

	private:
		Brochure* brochure;
	};

	class Brochure::Resources
	{
	public:
		explicit Resources(const Brochure& brochure);
		~Resources() = default;

		struct const_iterator;
		const_iterator by_name(
			const std::string& name,
			const std::string& type = std::string()) const;
		const_iterator by_fuzzy_name(
			const std::string& name,
			const std::string& type = std::string()) const;
		const_iterator by_type(const std::string& type) const;

		bool has(const Resource& resource) const;
		bool has(int id) const;

		Resource get(int id) const;

		const_iterator begin() const;
		const_iterator end() const;

	private:
		const Brochure* brochure;
	};

	class Brochure::Resources::const_iterator :
		public std::iterator<
			std::input_iterator_tag,
			Resource,
			std::size_t,
			const Resource*,
			const Resource&>
	{
	public:
		friend class Resources;

		const_iterator() = default;
		~const_iterator();

		const_iterator& operator ++();
		const_iterator operator ++(int);
		bool operator ==(const const_iterator& other) const;
		bool operator !=(const const_iterator& other) const;

		pointer operator ->() const;
		value_type operator *() const;

	private:
		explicit const_iterator(Statement& statement);
		void next();

		std::shared_ptr<Statement> statement;
		bool end = true;
		Resource value;
	};

	class Brochure::Actions
	{
	public:
		Actions(const Brochure& brochure);
		Actions(const Brochure& brochure, const Resource& resource);

		struct const_iterator;
		const_iterator by_type(
			const std::string& type,
			const std::string& name = std::string()) const;
		const_iterator getters() const;

		const_iterator definitions() const;

		const_iterator begin() const;
		const_iterator end() const;

		bool has(const Action& action) const;
		bool has(int id) const;

		Action get(int id) const;

	private:
		const Brochure* brochure;
		Resource resource;
		bool is_limited;
	};

	class Brochure::Actions::const_iterator :
		public std::iterator<
			std::input_iterator_tag,
			Action,
			std::size_t,
			const Action*,
			const Action&>
	{
	public:
		friend class Actions;

		const_iterator() = default;
		~const_iterator();

		const_iterator& operator ++();
		const_iterator operator ++(int);
		bool operator ==(const const_iterator& other) const;
		bool operator !=(const const_iterator& other) const;

		pointer operator ->() const;
		value_type operator *() const;

	private:
		explicit const_iterator(Database& database, Statement& statement, bool is_instance = true);
		void next();

		Database* database;
		std::shared_ptr<Statement> statement;
		bool is_instance;
		bool end = true;
		Action value;
	};

	class Brochure::Requirements
	{
	public:
		Requirements(const Brochure& brochure, const Action& action);

		struct const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

	private:
		const Brochure* brochure;
		Action action;
	};

	class Brochure::Requirements::const_iterator :
		public std::iterator<
			std::input_iterator_tag,
			Requirement,
			std::size_t,
			const Requirement*,
			const Requirement&>
	{
	public:
		friend class Requirements;

		const_iterator() = default;
		~const_iterator();

		const_iterator& operator ++();
		const_iterator operator ++(int);
		bool operator ==(const const_iterator& other) const;
		bool operator !=(const const_iterator& other) const;

		pointer operator ->() const;
		value_type operator *() const;

	private:
		explicit const_iterator(const Brochure& brochure, Statement& statement);
		void next();

		const Brochure* brochure;
		std::shared_ptr<Statement> statement;
		bool end = true;
		Requirement value;
	};
}

#endif
