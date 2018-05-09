// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "plato/import/importer.hpp"

int plato::Importer::import_from_file(
	const std::string& filename,
	Importer& importer,
	twoflower::Brochure& brochure)
{
	return import_from_file(filename, importer, brochure, [](auto) { return true; });
}

int plato::Importer::import_from_file(
	const std::string& filename,
	Importer& importer,
	twoflower::Brochure& brochure,
	const std::function<bool(int)>& callback)
{
	std::ifstream stream(filename);
	Json::Value root;
	stream >> root;

	int count = 0;
	for (auto& i: root)
	{
		count += importer.import(i, brochure);
		if (!callback(count))
		{
			break;
		}
	}

	return count;
}
