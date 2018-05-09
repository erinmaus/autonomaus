// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_TRACE_TRACE_HPP
#define TOUCHSTONE_TRACE_TRACE_HPP

#include <vector>

namespace touchstone
{
	class Call;
	class CallStream;

	class Trace
	{
	public:
		Trace() = default;
		virtual ~Trace();

		void extract(CallStream stream);
		std::size_t extract_single_call(CallStream stream, std::size_t offset);
		void clear();

		std::size_t count() const;
		const Call* at(std::size_t index) const;

	protected:
		virtual Call* read_call(
			std::size_t call_num,
			CallStream& stream) = 0;

	private:
		std::size_t current_call = 0;

		typedef std::vector<Call*> CallCollection;
		CallCollection calls;

	public:
		typedef CallCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
