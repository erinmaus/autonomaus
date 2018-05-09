// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMMON_UTILITY_LIGHT_SHARED_POINTER_HPP
#define KOMMON_UTILITY_LIGHT_SHARED_POINTER_HPP

#include <cstdio>
#include <cassert>
#include <utility>

namespace kommon
{
	template <typename Type>
	class LightSharedPointer
	{
	public:
		typedef Type element_type;

		LightSharedPointer();
		LightSharedPointer(Type* value);
		LightSharedPointer(const LightSharedPointer& other);
		LightSharedPointer(LightSharedPointer&& other);
		~LightSharedPointer();

		Type* get() const;

		LightSharedPointer& operator =(const LightSharedPointer& other);
		LightSharedPointer& operator =(LightSharedPointer&& other);
		LightSharedPointer& operator =(Type&& other);

		Type& operator *() const;
		Type* operator ->() const;

		bool operator <(const LightSharedPointer& other) const;
		bool operator <=(const LightSharedPointer& other) const;
		bool operator >(const LightSharedPointer& other) const;
		bool operator >=(const LightSharedPointer& other) const;
		bool operator ==(const LightSharedPointer& other) const;
		bool operator !=(const LightSharedPointer& other) const;

		std::size_t use_count() const;

		operator bool() const;

	private:
		void release();

		struct Data
		{
		public:
			Data(Type* value);
			~Data();

			std::size_t count = 1;
			Type* value;
		};

		mutable Data* data;
	};
}

template <typename Type>
inline kommon::LightSharedPointer<Type>::LightSharedPointer()
{
	this->data = nullptr;
}

template <typename Type>
inline kommon::LightSharedPointer<Type>::LightSharedPointer(Type* value)
{
	this->data = new Data(value);
}

template <typename Type>
inline kommon::LightSharedPointer<Type>::LightSharedPointer(
	const LightSharedPointer& other)
{
	this->data = other.data;
	if (this->data != nullptr)
	{
		++this->data->count;
	}
}

template <typename Type>
inline kommon::LightSharedPointer<Type>::LightSharedPointer(
	LightSharedPointer&& other)
{
	this->data = other.data;
	other.data = nullptr;
}

template <typename Type>
inline kommon::LightSharedPointer<Type>::~LightSharedPointer()
{
	release();
}

template <typename Type>
Type* kommon::LightSharedPointer<Type>::get() const
{
	return this->data->value;
}

template <typename Type>
inline kommon::LightSharedPointer<Type>&
kommon::LightSharedPointer<Type>::operator =(const LightSharedPointer& other)
{
	release();

	this->data = other.data;
	if (this->data != nullptr)
	{
		++this->data->count;
	}

	return *this;
}

template <typename Type>
inline kommon::LightSharedPointer<Type>&
kommon::LightSharedPointer<Type>::operator =(LightSharedPointer&& other)
{
	release();

	this->data = other.data;
	other.data = nullptr;

	return *this;
}

template <typename Type>
inline Type& kommon::LightSharedPointer<Type>::operator *() const
{
	return *this->data->value;
}

template <typename Type>
inline Type* kommon::LightSharedPointer<Type>::operator ->() const
{
	return this->data->value;
}

template <typename Type>
inline bool kommon::LightSharedPointer<Type>::operator <(
	const LightSharedPointer& other) const
{
	return this->data < other.data;
}

template <typename Type>
inline bool kommon::LightSharedPointer<Type>::operator <=(
	const LightSharedPointer& other) const
{
	return this->data <= other.data;
}

template <typename Type>
inline bool kommon::LightSharedPointer<Type>::operator >(
	const LightSharedPointer& other) const
{
	return this->data > other.data;
}

template <typename Type>
inline bool kommon::LightSharedPointer<Type>::operator >=(
	const LightSharedPointer& other) const
{
	return this->data >= other.data;
}

template <typename Type>
inline bool kommon::LightSharedPointer<Type>::operator ==(
	const LightSharedPointer& other) const
{
	return this->data == other.data;
}

template <typename Type>
inline bool kommon::LightSharedPointer<Type>::operator !=(
	const LightSharedPointer& other) const
{
	return this->data != other.data;
}

template <typename Type>
std::size_t kommon::LightSharedPointer<Type>::use_count() const
{
	if (this->data != nullptr)
	{
		return this->data->count;
	}

	return 0;
}

template <typename Type>
inline kommon::LightSharedPointer<Type>::operator bool() const
{
	return data != nullptr;
}

template <typename Type>
inline void kommon::LightSharedPointer<Type>::release()
{
	if (this->data != nullptr)
	{
		assert(this->data->count > 0);

		--this->data->count;
		if (this->data->count == 0)
		{
			delete this->data;
			this->data = nullptr;
		}
	}
}

template <typename Type>
inline kommon::LightSharedPointer<Type>::Data::Data(Type* value)
{
	this->value = value;
}

template <typename Type>
inline kommon::LightSharedPointer<Type>::Data::~Data()
{
	delete this->value;
}

#endif
