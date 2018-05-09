// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_BUFFER_DATA_HPP
#define KOMPVTER_GRAPHICS_BUFFER_DATA_HPP

#include <cstdint>
#include <map>
#include <mutex>
#include <queue>

namespace kompvter
{
	class BufferData
	{
	public:
		BufferData() = default;
		explicit BufferData(std::size_t size);
		BufferData(const BufferData& other);
		~BufferData();

		void update(
			std::size_t offset, const std::uint8_t* data, std::size_t size);
		void clear();

		const std::uint8_t* get_data() const;
		std::uint8_t* get_data();
		std::size_t get_data_size() const;

		BufferData& operator =(const BufferData& other);

	private:
		struct Pool
		{
		public:
			std::uint8_t* allocate(std::size_t size);
			void deallocate(std::uint8_t* data, std::size_t size);

		private:
			static std::size_t next_power_of_two(std::size_t value);
			typedef std::queue<std::uint8_t*> AllocationQueue;
			std::map<std::size_t /* size (power-of-two) */, AllocationQueue> bins;
			std::mutex mutex;
		};
		static Pool pool;

		typedef std::uint8_t* DataPointer;
		DataPointer data;
		std::size_t data_size = 0;
	};
}

#endif
