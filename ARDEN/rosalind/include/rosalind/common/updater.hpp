// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_COMMON_UPDATER_HPP
#define ROSALIND_COMMON_UPDATER_HPP

#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "rosalind/processors/bufferProcessor.hpp"
#include "rosalind/processors/frameProcessor.hpp"
#include "rosalind/processors/guiProcessor.hpp"
#include "rosalind/processors/itemIconProcessor.hpp"
#include "rosalind/processors/modelProcessor.hpp"
#include "rosalind/processors/minimapProcessor.hpp"
#include "rosalind/processors/textureProcessor.hpp"
#include "touchstone/trace/trace.hpp"

namespace rosalind
{
	class Updater
	{
	public:
		Updater(
			kompvter::BufferManager& buffer_manager,
			kompvter::TextureManager& texture_manager);
		~Updater() = default;

		void update(const touchstone::Trace& trace, float timestamp);
		void enable_draw(float timestamp);
		void disable_draw(float timestamp);

		BufferProcessor& get_buffer_processor();
		const BufferProcessor& get_buffer_processor() const;
		TextureProcessor& get_texture_processor();
		const TextureProcessor& get_texture_processor() const;
		FrameProcessor& get_frame_processor();
		const FrameProcessor& get_frame_processor() const;
		GUIProcessor& get_gui_processor();
		const GUIProcessor& get_gui_processor() const;
		ItemIconProcessor& get_item_icon_processor();
		const ItemIconProcessor& get_item_icon_processor() const;
		MinimapProcessor& get_minimap_processor();
		const MinimapProcessor& get_minimap_processor() const;
		ModelProcessor& get_model_processor();
		const ModelProcessor& get_model_processor() const;

	private:
		touchstone::Context context;
		rosalind::BufferProcessor buffer_processor;
		rosalind::TextureProcessor texture_processor;
		rosalind::FrameProcessor frame_processor;
		rosalind::GUIProcessor gui_processor;
		rosalind::ItemIconProcessor item_icon_processor;
		rosalind::MinimapProcessor minimap_processor;
		rosalind::ModelProcessor model_processor;
	};
}

#endif
