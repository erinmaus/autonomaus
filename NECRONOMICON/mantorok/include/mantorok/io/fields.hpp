// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_FIELDS_HPP
#define MANTOROK_IO_FIELDS_HPP

#include <iostream>
#include <cstddef>
#include <cstdint>

namespace mantorok
{
	namespace detail
	{
		template <typename Type>
		struct FieldToken
		{
		};

		template <>
		struct FieldToken<std::uint8_t>
		{
			static const std::int32_t value = 'UBYT';
		};

		template <>
		struct FieldToken<std::int8_t>
		{
			static const std::int32_t value = 'SBYT';
		};

		template <>
		struct FieldToken<std::uint16_t>
		{
			static const std::int32_t value = 'USHO';
		};

		template <>
		struct FieldToken<std::int16_t>
		{
			static const std::int32_t value = 'SSHO';
		};

		template <>
		struct FieldToken<std::uint32_t>
		{
			static const std::int32_t value = 'UINT';
		};

		template <>
		struct FieldToken<std::int32_t>
		{
			static const std::int32_t value = 'SINT';
		};

		template <>
		struct FieldToken<float>
		{
			static const std::int32_t value = 'SING';
		};

		template <>
		struct FieldToken<double>
		{
			static const std::int32_t value = 'DOUB';
		};

		template <>
		struct FieldToken<std::size_t>
		{
			static const std::int32_t value = 'UINT';
		};

		template <>
		struct FieldToken<std::string>
		{
			static const std::int32_t value = 'STRI';
		};

		template <>
		struct FieldToken<std::uint8_t*>
		{
			static const std::int32_t value = 'DATA';
		};

		template <typename Type>
		struct FieldWriterReader
		{
			static void write(Type value, std::ostream& stream);
			static void read(Type& value, std::istream& stream);
		};

		template <>
		struct FieldWriterReader<std::size_t>
		{
			static void write(std::size_t value, std::ostream& stream);
			static void read(std::size_t& value, std::istream& stream);
		};

		template <>
		struct FieldWriterReader<std::string>
		{
			static void write(const std::string& value, std::ostream& stream);
			static void read(std::string& value, std::istream& stream);
		};
	}
}

template <typename Type>
void mantorok::detail::FieldWriterReader<Type>::write(
	Type value, std::ostream& stream)
{
	stream.write((const char*)&value, sizeof(Type));
}

template <typename Type>
void mantorok::detail::FieldWriterReader<Type>::read(
	Type& value, std::istream& stream)
{
	stream.read((char*)&value, sizeof(Type));
}

#endif
