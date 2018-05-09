// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_TEXTURE_DEFINITION_HPP
#define CHOMSKY_RESOURCES_TEXTURE_DEFINITION_HPP

#include <string>
#include <set>
#include <map>
#include "chomsky/resources/textureFingerprint.hpp"

namespace chomsky
{
	class TextureDefinition
	{
	public:
		TextureDefinition() = default;
		explicit TextureDefinition(
			const std::string& key,
			const TextureFingerprint& fingerprint);
		~TextureDefinition() = default;

		const std::string& get_key() const;
		const TextureFingerprint& get_fingerprint() const;

		const std::string& get_value() const;
		void set_value(const std::string& value);

		void add_tag(const std::string& tag);
		void remove_tag(const std::string& tag);
		bool has_tag(const std::string& tag) const;

		void add_data(const std::string& key, const std::string& value);
		void remove_data(const std::string& key);
		std::string get_data(const std::string& key) const;
		bool has_data(const std::string& key) const;

	private:
		std::string key;
		TextureFingerprint fingerprint;

		std::string value;

		std::set<std::string> tags;
		std::map<std::string, std::string> data;

	public:
		typedef std::set<std::string>::const_iterator const_tag_iterator;

		const_tag_iterator tags_begin() const;
		const_tag_iterator tags_end() const;

		typedef std::map<std::string, std::string>::const_iterator const_data_iterator;
		const_data_iterator data_begin() const;
		const_data_iterator data_end() const;
	};
}

#endif
