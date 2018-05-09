// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/trace/call.hpp"
#include "touchstone/trace/discreteCall.hpp"

touchstone::DiscreteCall::DiscreteCall(const Call* call) :
	data(new Data(call))
{
	// Nothing.
}

const touchstone::Call* touchstone::DiscreteCall::get() const
{
	if (this->data)
	{
		return this->data->call.get();
	}

	return nullptr;
}

const touchstone::Call& touchstone::DiscreteCall::operator *() const
{
	return *get();
}

const touchstone::Call* touchstone::DiscreteCall::operator ->() const
{
	return get();
}

bool touchstone::DiscreteCall::operator <(const DiscreteCall& other) const
{
	if (!*this)
	{
		if (!other)
		{
			return false;
		}
	}
	else
	{
		if (!other)
		{
			return false;
		}
	}

	return this->get()->get_call_num() < other.get()->get_call_num();
}

bool touchstone::DiscreteCall::operator <=(const DiscreteCall& other) const
{
	return (*this < other) || (*this == other);
}

bool touchstone::DiscreteCall::operator >(const DiscreteCall& other) const
{
	if (!*this)
	{
		if (!other)
		{
			return true;
		}
	}
	else
	{
		if (!other)
		{
			return true;
		}
	}

	return this->get()->get_call_num() > other.get()->get_call_num();
}

bool touchstone::DiscreteCall::operator >=(const DiscreteCall& other) const
{
	return (*this > other) || (*this == other);
}

bool touchstone::DiscreteCall::operator ==(const DiscreteCall& other) const
{
	if (this->data.get() == nullptr || other.data.get() == nullptr)
	{
		return this->data.get() == nullptr && other.data.get() == nullptr;
	}
	else
	{
		return this->get()->get_call_num() == other.get()->get_call_num();
	}
}

bool touchstone::DiscreteCall::operator !=(const DiscreteCall& other) const
{
	return !(*this == other);
}

touchstone::DiscreteCall::operator bool() const
{
	return get() != nullptr;
}

touchstone::DiscreteCall::Data::Data(const Call* call)
{
	this->call_data = std::make_unique<std::uint8_t[]>(call->get_size());
	std::memcpy(this->call_data.get(), call->get_data(), call->get_size());

	this->call = std::unique_ptr<Call>(call->clone());
	{
		CallStream stream(call->get_data(), call->get_size());
		this->call->update(stream);
	}
}
