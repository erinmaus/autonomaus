// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_IMPORT_SHO_IMPORTER_HPP
#define PLATO_IMPORT_SHO_IMPORTER_HPP

#include "plato/import/importer.hpp"

namespace plato
{
	class ShopImporter : public Importer
	{
	public:
		int import(
			const Json::Value& value,
			twoflower::Brochure& brochure) override;
	};
}

#endif
