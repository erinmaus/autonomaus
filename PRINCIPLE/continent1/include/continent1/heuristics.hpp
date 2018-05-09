// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CONTINENT1_HEURISTICS_HPP
#define CONTINENT1_HEURISTICS_HPP

#include "continent1/candidateManager.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace continent1 { namespace heuristics
{
	bool is_tile_impassable(
		const thoreau::TileCoordinate& tile,
		const CandidateManager& candidate_manager);
	bool is_tile_sparse(
		const thoreau::TileCoordinate& tile,
		const CandidateManager& candidate_manager);
} }

#endif
