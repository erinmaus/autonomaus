// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_MOUSE_MOUSE_PATH_HPP
#define EIZNEBAHN_MOUSE_MOUSE_PATH_HPP

#include <tuple>
#include <vector>

namespace eiznebahn
{
	class MousePath
	{
	public:
		MousePath() = default;
		~MousePath() = default;

		static const std::size_t X = 0;
		static const std::size_t Y = 1;
		static const std::size_t DELAY = 2;
		typedef std::tuple<int, int, float> Point;

		void add(int x, int y, float delay);
		Point get(std::size_t index) const;

		bool empty() const;
		std::size_t count() const;

		int get_length() const;
		float get_direction() const;
		float get_duration() const;
		float get_drift() const;

		void transform(float length, float direction);
		void translate(int x, int y);
		void time(float multiplier);

	private:
		typedef std::vector<Point> PointCollection;
		PointCollection points;
		float duration = 0.0f;

		mutable bool is_dirty = false;
		mutable float drift = 0.0f;
		void refresh() const;

	public:
		typedef PointCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
