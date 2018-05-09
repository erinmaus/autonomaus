// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_TEXTURE_KEY_MANAGER_HPP
#define CHOMSKY_RESOURCES_TEXTURE_KEY_MANAGER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "chomsky/resources/textureDictionary.hpp"
#include "chomsky/resources/textureFingerprintManager.hpp"
#include "kompvter/events/textureEvent.hpp"
#include "kompvter/listeners/textureListener.hpp"
#include "kompvter/graphics/textureManager.hpp"

namespace chomsky
{
	class TextureKeyManager : public kompvter::TextureListener
	{
	public:
		TextureKeyManager(
			const kompvter::TextureManager& texture_manager,
			const TextureFingerprintManager& texture_fingerprint_manager,
			const TextureDictionary& texture_dictionary);
		~TextureKeyManager() = default;

		float get_max_difference() const;
		void set_max_difference(float value);

		float get_max_dissimilarity() const;
		void set_max_dissimilarity(float value);

		void on_texture_delete(const kompvter::TextureEvent& event) override;
		void on_texture_update(const kompvter::TextureEvent& event) override;
		void on_texture_copy(const kompvter::TextureEvent& event) override;

		void reset();

		bool has(int texture_name, int region_name) const;
		std::string get(int texture_name, int region_name) const;

		void add(
			int texture_name,
			int x, int y, int width, int height,
			const std::string& key);
		void add(int texture_name, int region_name, const std::string& key);
		void remove(const std::string& key);

	private:
		void add_pending(
			int texture_name,
			int x, int y, int width, int height);
		bool update_pending(
			int texture_name, int region_name,
			std::string& result) const;
		void remove_any(int texture_name, int x, int y, int width, int height);

		float max_difference = 1.0f;

		// Selected by a rigid scientific process.
		//
		// (In other words, this is just a reasonable value. It has no basis other
		// than working thus far).
		float max_dissimilarity = 16.0f;

		const kompvter::TextureManager* texture_manager;
		const TextureFingerprintManager* texture_fingerprint_manager;
		const TextureDictionary* texture_dictionary;

		struct TextureUpdate
		{
			int x, y, width, height;
			bool ready = false;
			std::string key;
		};

		typedef std::unordered_map<int, TextureUpdate> TextureUpdateCollection;
		mutable std::unordered_map<int, TextureUpdateCollection> updates;
	};
}

#endif
