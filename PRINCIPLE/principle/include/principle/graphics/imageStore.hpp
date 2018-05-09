// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_GRAPHICS_IMAGE_STORE_HPP
#define PRINCIPLE_GRAPHICS_IMAGE_STORE_HPP

#include <cstddef>
#include <fstream>
#include <memory>
#include <string>

namespace principle
{
	template <typename Element, std::size_t Count, int Width = 64, int Height = 64>
	class ImageStore
	{
	public:
		static const int WIDTH = Width;
		static const int HEIGHT = Height;
		static const int BLOCK_SIZE = WIDTH * HEIGHT;
		static const int COUNT = Count;

		ImageStore() = default;
		~ImageStore() = default;

		void read(const std::string& filename);
		void read(const std::string& filename, std::size_t index);
		void write(const std::string& filename) const;
		void write(const std::string& filename, std::size_t index) const;

		Element* get(std::size_t index);

		void clear(const Element& value);
		void clear(std::size_t index, const Element& value);

	private:
		typedef std::unique_ptr<Element[]> BlockPointer;
		BlockPointer blocks[COUNT];
	};
}

template <typename Element, std::size_t Count, int Width, int Height>
void principle::ImageStore<Element, Count, Width, Height>::read(
	const std::string& filename)
{
	std::ifstream stream(filename, std::ios::binary);
	for (std::size_t i = 0; i < COUNT; ++i)
	{
		stream.read((char*)get(i), WIDTH * HEIGHT * sizeof(Element));
	}
}

template <typename Element, std::size_t Count, int Width, int Height>
void principle::ImageStore<Element, Count, Width, Height>::read(
	const std::string& filename, std::size_t index)
{
	std::ifstream stream(filename, std::ios::binary);
	stream.seekg(index * WIDTH * HEIGHT * sizeof(Element));
	stream.read((char*)get(index), WIDTH * HEIGHT * sizeof(Element));
}

template <typename Element, std::size_t Count, int Width, int Height>
void principle::ImageStore<Element, Count, Width, Height>::write(
	const std::string& filename) const
{
	std::ofstream stream(filename, std::ios::binary);
	for (std::size_t i = 0; i < COUNT; ++i)
	{
		assert(this->blocks[i].get() != nullptr);
		stream.write((const char*)this->blocks[i].get(), WIDTH * HEIGHT * sizeof(Element));
	}
}

template <typename Element, std::size_t Count, int Width, int Height>
void principle::ImageStore<Element, Count, Width, Height>::write(
	const std::string& filename, std::size_t index) const
{
	assert(this->blocks[index].get() != nullptr);
	std::fstream stream(filename, std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);
	stream.seekp(index * WIDTH * HEIGHT * sizeof(Element));
	stream.write((const char*)this->blocks[index].get(), WIDTH * HEIGHT * sizeof(Element));
}

template <typename Element, std::size_t Count, int Width, int Height>
Element* principle::ImageStore<Element, Count, Width, Height>::get(std::size_t index)
{
	assert(index < COUNT);
	if (!this->blocks[index])
	{
		this->blocks[index] = std::make_unique<Element[]>(BLOCK_SIZE);
	}

	return this->blocks[index].get();
}

template <typename Element, std::size_t Count, int Width, int Height>
void principle::ImageStore<Element, Count, Width, Height>::clear(const Element& value)
{
	for (std::size_t i = 0; i < COUNT; ++i)
	{
		clear(i, value);
	}
}

template <typename Element, std::size_t Count, int Width, int Height>
void principle::ImageStore<Element, Count, Width, Height>::clear(std::size_t index, const Element& value)
{
	auto element = get(index);
	for (std::size_t i = 0; i < WIDTH * HEIGHT; ++i)
	{
		element[i] = value;
	}
}

#endif
