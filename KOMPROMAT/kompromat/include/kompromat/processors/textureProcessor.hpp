// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_PROCESSORS_TEXTURE_PROCESSOR_HPP
#define KOMPROMAT_PROCESSORS_TEXTURE_PROCESSOR_HPP

#include "kompromat/listenerProcessor.hpp"
#include "kompvter/listeners/textureListener.hpp"

namespace kompromat
{
	class TextureEvent;

	class TextureProcessor : public ListenerProcessor<kompvter::TextureListener>
	{
	public:
		TextureProcessor() = default;
		~TextureProcessor() = default;

		void process(
			const std::string& scope,
			int name,
			const paley::Event& event) override;

	private:
		void process_texture_create(const paley::Event& event);
		void process_texture_delete(const paley::Event& event);
		void process_texture_allocate(const paley::Event& event);
		void process_texture_update(const paley::Event& event);
		void process_texture_copy(const paley::Event& event);
	};
}

#endif
