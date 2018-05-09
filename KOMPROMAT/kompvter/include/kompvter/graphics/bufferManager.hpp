// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_BUFFER_MANAGER_HPP
#define KOMPVTER_GRAPHICS_BUFFER_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include "kommon/utility/lightSharedPointer.hpp"
#include "kompvter/graphics/buffer.hpp"
#include "kompvter/graphics/bufferData.hpp"
#include "kompvter/listeners/bufferListener.hpp"

namespace kompvter
{
	class BufferManager : public BufferListener
	{
	public:
		BufferManager() = default;
		BufferManager(const BufferManager& other) = delete;
		~BufferManager() = default;

		bool has_buffer(int name) const;
		Buffer get_buffer(int name) const;

		void reset();

		void on_buffer_create(const BufferEvent& event) override;
		void on_buffer_delete(const BufferEvent& event) override;
		void on_buffer_allocate(const BufferEvent& event) override;
		void on_buffer_clear(const BufferEvent& event) override;
		void on_buffer_update(const BufferEvent& event) override;

		BufferManager& operator = (const BufferManager& other) = delete;

	private:
		typedef std::shared_ptr<BufferData> BufferDataPointer;
		typedef std::unordered_map<int, BufferDataPointer> BufferDataMap;
		BufferDataMap buffers;

	public:
		struct const_iterator : public BufferDataMap::const_iterator
		{
		public:
			typedef std::pair<int, Buffer> value_type;

			const_iterator(const BufferDataMap::const_iterator& other);
			~const_iterator() = default;

			value_type* operator ->() const;
			value_type& operator *() const;

		private:
			mutable value_type current;
		};

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
