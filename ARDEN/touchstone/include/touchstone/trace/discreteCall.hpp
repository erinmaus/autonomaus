// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_TRACE_DISCRETE_CALL_HPP
#define TOUCHSTONE_TRACE_DISCRETE_CALL_HPP

#include <cstdint>
#include <memory>
#include "touchstone/detail/lightSharedPointer.hpp"
#include "touchstone/trace/call.hpp"

namespace touchstone
{
	class Call;

	class DiscreteCall
	{
	public:
		DiscreteCall() = default;
		DiscreteCall(const Call* call);
		DiscreteCall(DiscreteCall&& other) = default;
		DiscreteCall(const DiscreteCall& other) = default;
		~DiscreteCall() = default;

		const Call* get() const;

		const Call& operator *() const;
		const Call* operator ->() const;

		DiscreteCall& operator =(const DiscreteCall& other) = default;
		DiscreteCall& operator =(DiscreteCall&& other) = default;

		// Empty calls go first. Otherwise, sort by call number.
		bool operator <(const DiscreteCall& other) const;
		bool operator >(const DiscreteCall& other) const;
		bool operator <=(const DiscreteCall& other) const;
		bool operator >=(const DiscreteCall& other) const;
		bool operator ==(const DiscreteCall& other) const;
		bool operator !=(const DiscreteCall& other) const;

		operator bool() const;

	private:
		struct Data
		{
		public:
			friend class DiscreteCall;

			Data() = default;
			Data(const Call* call);
			~Data() = default;

		private:
			std::unique_ptr<Call> call;
			std::unique_ptr<std::uint8_t[]> call_data;
		};

		typedef LightSharedPointer<Data> DataPointer;
		DataPointer data;
	};
}

#endif
