// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_IMPORT_IMPORTER_HPP
#define PLATO_IMPORT_IMPORTER_HPP

#include <functional>
#include <jsoncpp/json/json.h>
#include "twoflower/brochure.hpp"

namespace plato
{
	class Importer
	{
	public:
		virtual ~Importer() = default;
		virtual int import(
			const Json::Value& value,
			twoflower::Brochure& brochure) = 0;

		static int import_from_file(
			const std::string& filename,
			Importer& importer,
			twoflower::Brochure& brochure);
		static int import_from_file(
			const std::string& filename,
			Importer& importer,
			twoflower::Brochure& brochure,
			const std::function<bool(int)>& callback);
	};
}

#endif
