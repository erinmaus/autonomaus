// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_RELATIONSHIPS_RESOURCE_HPP
#define TWOFLOWER_RELATIONSHIPS_RESOURCE_HPP

#include <string>

namespace twoflower
{
	class Resource
	{
	public:
		Resource() = default;
		~Resource() = default;

		class Builder;
		Builder builder();

		int get_id() const;
		const std::string& get_name() const;
		const std::string& get_type() const;

	private:
		int id = 0;
		std::string type;
		std::string name;
	};

	class Resource::Builder
	{
	public:
		Builder(Resource& resource);

		void set_id(int value);
		void set_type(const std::string& value);
		void set_name(const std::string& value);

	private:
		Resource* resource;
	};
}

#endif
