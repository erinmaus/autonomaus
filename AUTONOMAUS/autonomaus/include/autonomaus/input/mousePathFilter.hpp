// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_INPUT_MOUSE_PATH_FILTER_HPP
#define AUTONOMAUS_INPUT_MOUSE_PATH_FILTER_HPP

#include <cmath>
#include <functional>
#include "eiznebahn/mouse/mousePath.hpp"

namespace autonomaus
{
	class MousePathFilter
	{
	public:
		MousePathFilter() = default;
		~MousePathFilter() = default;

		/// Adds the paths obtained from the provided range.
		///
		/// The input paths must not be destroyed before the filter.
		template <typename InputIter>
		void collect(InputIter begin, InputIter end);

		void clear();

		/// Prepares the query.
		///
		/// This must be called prior to filter methods or selection methods.
		///
		/// Discards the current query, whether one is being built or was
		/// finalized.
		void start(std::size_t min = 1);

		/// Finalizes the current query.
		///
		/// This must be called after the query is complete, before retrieving a
		/// path.
		///
		/// Returns a boolean indicating if any matches were found.
		bool stop();

		/// Discards all but the first 'count' paths in the current query.
		///
		/// If 'count' is less than the 'min' value passed to start, no paths
		/// are discarded and 'select' returns false.
		///
		/// Generally, the return value of the sort methods is used as input.
		bool select(std::size_t count);

		/// Sorts the paths by direction.
		///
		/// Inputs are in radians, from 0 to 2*pi.
		///
		/// Returns the number of paths within 'threshold' radians of 'value'.
		std::size_t direction(float value, float threshold = HUGE_VALF);

		/// Sorts the paths by length.
		///
		/// Inputs are in pixels.
		///
		/// Returns the number of paths within 'threshold' pixels of 'value'.
		std::size_t length(float value, float threshold = HUGE_VALF);

		/// Sorts the paths by the drift:length ratio.
		///
		/// Drift is defined by the total sum of points' distance from the line
		/// formed by a the first and last points in a eiznebahn::MousePath. A
		/// value of 0 is a perfectly straight line (i.e., all points are on the
		/// aforementioned line), while larger values indicate a greater
		/// divergence from this line.
		///
		/// Returns the number of paths within 'threshold' pixels of 'value'.
		std::size_t drift(float value, float threshold = HUGE_VALF);

		/// Sorts the paths by duration.
		std::size_t duration(float value);

		/// Returns the number of paths in the query.
		std::size_t count() const;

		/// Gets a path at 'index'.
		///
		/// This can only be called after the path has been finalized. See 'stop'.
		const eiznebahn::MousePath& get(std::size_t index) const;

	private:
		std::size_t filter(
			const std::function<float(const eiznebahn::MousePath& path)>& get,
			float value, float threshold);

		static const eiznebahn::MousePath EMPTY;
		std::vector<const eiznebahn::MousePath*> paths;
		std::size_t current = 0;
		std::size_t limit = 0;
	};
}

template <typename InputIter>
void autonomaus::MousePathFilter::collect(
	InputIter begin, InputIter end)
{
	for (auto i = begin; i != end; ++i)
	{
		this->paths.push_back(&(*i));
	}
}

#endif
