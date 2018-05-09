// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/resources/textureDefinition.hpp"

chomsky::TextureDefinition::TextureDefinition(
	const std::string& key,
	const TextureFingerprint& fingerprint)
{
	this->key = key;
	this->fingerprint = fingerprint;
}

const std::string& chomsky::TextureDefinition::get_key() const
{
	return this->key;
}

const chomsky::TextureFingerprint& chomsky::TextureDefinition::get_fingerprint() const
{
	return this->fingerprint;
}

const std::string& chomsky::TextureDefinition::get_value() const
{
	return this->value;
}

void chomsky::TextureDefinition::set_value(const std::string& value)
{
	this->value = value;
}

void chomsky::TextureDefinition::add_tag(const std::string& tag)
{
	this->tags.insert(tag);
}

void chomsky::TextureDefinition::remove_tag(const std::string& tag)
{
	this->tags.erase(tag);
}

void chomsky::TextureDefinition::add_data(
	const std::string& key, const std::string& value)
{
	this->data[key] = value;
}

void chomsky::TextureDefinition::remove_data(const std::string& key)
{
	this->data.erase(key);
}

std::string chomsky::TextureDefinition::get_data(const std::string& key) const
{
	auto iter = this->data.find(key);
	if (iter != this->data.end())
	{
		return iter->second;
	}

	return std::string();
}

bool chomsky::TextureDefinition::has_data(const std::string& key) const
{
	return this->data.count(key) != 0;
}

bool chomsky::TextureDefinition::has_tag(const std::string& tag) const
{
	return this->tags.count(tag) != 0;
}

chomsky::TextureDefinition::const_tag_iterator
chomsky::TextureDefinition::tags_begin() const
{
	return this->tags.begin();
}

chomsky::TextureDefinition::const_tag_iterator
chomsky::TextureDefinition::tags_end() const
{
	return this->tags.end();
}

chomsky::TextureDefinition::const_data_iterator
chomsky::TextureDefinition::data_begin() const
{
	return this->data.begin();
}

chomsky::TextureDefinition::const_data_iterator
chomsky::TextureDefinition::data_end() const
{
	return this->data.end();
}
