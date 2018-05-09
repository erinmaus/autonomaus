// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/query/materialDefinition.hpp"

const chomsky::TextureFingerprint& chomsky::MaterialDefinition::get_fingerprint() const
{
	return this->fingerprint;
}

void chomsky::MaterialDefinition::set_fingerprint(const TextureFingerprint& value)
{
	this->fingerprint = value;
}

const glm::vec3& chomsky::MaterialDefinition::get_max_difference_threshold(void) const
{
	return this->max_difference_threshold;
}

void chomsky::MaterialDefinition::set_max_difference_threshold(const glm::vec3& value)
{
	this->max_difference_threshold = value;
}

const glm::vec3& chomsky::MaterialDefinition::get_max_dissimilarity(void) const
{
	return this->max_dissimilarity;
}

void chomsky::MaterialDefinition::set_max_dissimilarity(const glm::vec3& value)
{
	this->max_dissimilarity = value;
}

const glm::vec3& chomsky::MaterialDefinition::get_color() const
{
	return this->color;
}

void chomsky::MaterialDefinition::set_color(const glm::vec3& value)
{
	this->color = value;
}

const glm::vec3& chomsky::MaterialDefinition::get_max_color_difference() const
{
	return this->max_color_difference;
}

void chomsky::MaterialDefinition::set_max_color_difference(const glm::vec3& value)
{
	this->max_color_difference = value;
}
