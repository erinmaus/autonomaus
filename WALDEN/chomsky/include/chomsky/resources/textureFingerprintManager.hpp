// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_TEXTURE_FINGERPRINT_MANAGER_HPP
#define CHOMSKY_RESOURCES_TEXTURE_FINGERPRINT_MANAGER_HPP

#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>
#include "chomsky/resources/textureFingerprint.hpp"
#include "kommon/deps/ctpl/ctpl.h"
#include "kompvter/events/textureEvent.hpp"
#include "kompvter/listeners/textureListener.hpp"
#include "kompvter/graphics/textureManager.hpp"

namespace chomsky
{
	class TextureFingerprintManager : public kompvter::TextureListener
	{
	public:
		static const int DEFAULT_THREADS = 2;
		static const int DEFAULT_NUM_ROWS = 8;
		static const int DEFAULT_NUM_COLUMNS = 8;
		static const glm::vec3 DEFAULT_COLOR_RANGE;

		explicit TextureFingerprintManager(
			const kompvter::TextureManager& texture_manager);
		TextureFingerprintManager(
			const kompvter::TextureManager& texture_manager,
			int num_rows, int num_columns, const glm::vec3& color_range);
		~TextureFingerprintManager() = default;

		int get_num_rows() const;
		int get_num_columns() const;
		const glm::vec3& get_color_range() const;

		int get_num_threads() const;
		void set_num_threads(int count);

		bool pending() const;
		void wait() const;

		void on_texture_delete(const kompvter::TextureEvent& event) override;
		void on_texture_update(const kompvter::TextureEvent& event) override;
		void on_texture_copy(const kompvter::TextureEvent& event) override;

		void reset();

		bool ready(int texture_name, int region_name) const;
		bool has(int texture_name, int region_name) const;
		TextureFingerprint get(int texture_name, int region_name) const;

	private:
		void build_fingerprint(
			int texture_name,
			kompvter::TextureRegion region);

		void remove_any(int texture_name, int x, int y, int width, int height);

		const kompvter::TextureManager* texture_manager;
		int num_rows = DEFAULT_NUM_ROWS, num_columns = DEFAULT_NUM_COLUMNS;
		glm::vec3 color_range = glm::vec3(16.0f, 8.0f, 8.0f);

		mutable ctpl::thread_pool thread_pool;
		mutable std::mutex mutex;
		mutable std::condition_variable update_condition;
		bool use_threads = false;

		struct TextureUpdate
		{
			int x, y, width, height;
		};
		typedef std::map<int, TextureUpdate> TextureUpdateCollection;
		std::map<int, TextureUpdateCollection> updates;

		typedef std::pair<int, int> TextureFingerprintKey;
		std::set<TextureFingerprintKey> pending_fingerprints;
		std::map<TextureFingerprintKey, TextureFingerprint> fingerprints;
	};
}

#endif
