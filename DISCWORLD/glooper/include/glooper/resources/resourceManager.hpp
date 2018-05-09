// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_RESOURCES_RESOURCE_MANAGER_HPP
#define GLOOPER_RESOURCES_RESOURCE_MANAGER_HPP

#include <functional>
#include <map>
#include <string>
#include <vector>
#include "chomsky/gui/itemIconManager.hpp"
#include "chomsky/resources/textureDictionary.hpp"
#include "chomsky/resources/textureFingerprintManager.hpp"
#include "chomsky/resources/textureKeyManager.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "thoreau/map.hpp"

namespace glooper
{
	class ResourceListener;

	class ResourceManager
	{
	public:
		explicit ResourceManager(const std::string& path = "");
		ResourceManager(const ResourceManager& other) = delete;
		~ResourceManager() = default;

		const std::string& get_path() const;
		void set_path(const std::string& value);

		void for_each_directory(
			const std::string& path,
			bool recursive,
			const std::function<void(const std::string&)>& callback) const;
		void for_each_file(
			const std::string& path,
			const std::function<void(const std::string&)>& callback) const;

		kompvter::BufferManager& get_buffer_manager();
		const kompvter::BufferManager& get_buffer_manager() const;
		kompvter::TextureManager& get_texture_manager();
		const kompvter::TextureManager& get_texture_manager() const;

		chomsky::TextureFingerprintManager& get_texture_fingerprint_manager();
		const chomsky::TextureFingerprintManager& get_texture_fingerprint_manager() const;
		chomsky::TextureKeyManager& get_texture_key_manager();
		const chomsky::TextureKeyManager& get_texture_key_manager() const;

		chomsky::ItemIconManager& get_item_icon_manager();
		const chomsky::ItemIconManager& get_item_icon_manager() const;

		chomsky::TextureDictionary& get_texture_dictionary();
		const chomsky::TextureDictionary& get_texture_dictionary() const;

		thoreau::Map& get_map();
		const thoreau::Map& get_map() const;

		thoreau::Map& get_runtime_map();
		const thoreau::Map& get_runtime_map() const;

		void load_config(const std::string& filename);
		void save_config(const std::string& filename) const;

		float get_config_float(
			const std::string& scope, const std::string& key,
			float default_value = 0.0f) const;
		void set_config_float(
			const std::string& scope, const std::string& key,
			float value);

		int get_config_int(
			const std::string& scope, const std::string& key,
			int default_value = 0) const;
		void set_config_int(
			const std::string& scope, const std::string& key,
			int value);

		bool get_config_bool(
			const std::string& scope, const std::string& key,
			bool default_value = false) const;
		void set_config_bool(
			const std::string& scope, const std::string& key,
			bool value);
		void set_config_bool(
			const std::string& scope, const std::string& key,
			const std::string& value);

		std::string get_config_string(
			const std::string& scope, const std::string& key,
			const std::string& default_value = "") const;
		void set_config_string(
			const std::string& scope, const std::string& key,
			const std::string& value);
		
		void unset_config_entry(
			const std::string& scope, const std::string& key);

		void reload();

		void add_listener(ResourceListener* resource_listener);
		void remove_listener(ResourceListener* resource_listener);

		ResourceManager& operator =(const ResourceManager& other) = delete;

	private:
		static int load_ini(
			void* user,
			const char* section, const char* name,
			const char* value);

		// "true", "yes", "y", "on", "enable", "enabled", and "1" are valid
		static bool is_true_value(const std::string& value);
		// "false", "no", "n", "off", "disable", "disabled", and "0" are valid
		static bool is_false_value(const std::string& value);

		void load_texture_dictionary();
		void load_map();
		void load_special_items();

		std::string path;
		std::vector<ResourceListener*> listeners;

		typedef std::map<std::string, std::string> ConfigSection;
		std::map<std::string, ConfigSection> config;

		chomsky::TextureDictionary texture_dictionary;

		thoreau::Map map;
		thoreau::Map runtime_map;

		kompvter::BufferManager buffer_manager;
		kompvter::TextureManager texture_manager;
		chomsky::TextureFingerprintManager texture_fingerprint_manager;
		chomsky::TextureKeyManager texture_key_manager;
		chomsky::ItemIconManager item_icon_manager;
	};
}

#endif
