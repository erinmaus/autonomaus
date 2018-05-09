// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/resources/textureFingerprintManager.hpp"

const int chomsky::TextureFingerprintManager::DEFAULT_NUM_ROWS;
const int chomsky::TextureFingerprintManager::DEFAULT_NUM_COLUMNS;
const glm::vec3 chomsky::TextureFingerprintManager::DEFAULT_COLOR_RANGE = glm::vec3(16.0f, 8.0f, 8.0f);

chomsky::TextureFingerprintManager::TextureFingerprintManager(
	const kompvter::TextureManager& texture_manager) :
		thread_pool(DEFAULT_THREADS)
{
	this->texture_manager = &texture_manager;
}

chomsky::TextureFingerprintManager::TextureFingerprintManager(
	const kompvter::TextureManager& texture_manager,
	int num_rows, int num_columns, const glm::vec3& color_range)
{
	this->texture_manager = &texture_manager;
	this->num_rows = num_rows;
	this->num_columns = num_columns;
	this->color_range = color_range;
}

int chomsky::TextureFingerprintManager::get_num_rows() const
{
	return this->num_rows;
}

int chomsky::TextureFingerprintManager::get_num_columns() const
{
	return this->num_columns;
}

const glm::vec3& chomsky::TextureFingerprintManager::get_color_range() const
{
	return this->color_range;
}

int chomsky::TextureFingerprintManager::get_num_threads() const
{
	return this->thread_pool.size();
}

void chomsky::TextureFingerprintManager::set_num_threads(int value)
{
	if (value > 0)
	{
		this->use_threads = true;
		this->thread_pool.resize(value);
	}
	else
	{
		this->use_threads = false;
	}
}

bool chomsky::TextureFingerprintManager::pending() const
{
	if (!this->use_threads)
	{
		wait();
	}

	std::unique_lock<std::mutex> lock(this->mutex);
	return !this->pending_fingerprints.empty();
}

void chomsky::TextureFingerprintManager::wait() const
{
	std::unique_lock<std::mutex> lock(this->mutex);
	if (!this->pending_fingerprints.empty())
	{
		this->update_condition.wait(lock, [this]{ return this->pending_fingerprints.empty(); });
	}
}

void chomsky::TextureFingerprintManager::on_texture_delete(
	const kompvter::TextureEvent& event)
{
	this->updates.erase(event.get_name());
	for (auto i = this->fingerprints.begin(); i != this->fingerprints.end(); /* Nil. */)
	{
		auto key = i->first;
		if (key.first == event.get_name())
		{
			i = this->fingerprints.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void chomsky::TextureFingerprintManager::on_texture_update(
	const kompvter::TextureEvent& event)
{
	remove_any(
		event.get_destination_name(),
		event.get_destination_x(), event.get_destination_y(),
		event.get_width(), event.get_height());
	auto texture = this->texture_manager->get_texture(
		event.get_destination_name());
	auto region = texture.get_region_from_pixel(
		event.get_destination_x(),
		event.get_destination_y());

	if (region != nullptr)
	{
		std::lock_guard<std::mutex> lock(this->mutex);
		auto key = std::make_pair(
			event.get_destination_name(),
			region->get_name());
		this->pending_fingerprints.insert(key);

		auto region_copy_because_cpp_is_beautiful_slash_s = *region;
		int texture_name = texture.get_name();
		this->thread_pool.push(
			[this, texture_name, region_copy_because_cpp_is_beautiful_slash_s](auto)
			{
				this->build_fingerprint(texture_name, region_copy_because_cpp_is_beautiful_slash_s);
			});

		TextureUpdate update;
		update.x = event.get_destination_x();
		update.y = event.get_destination_y();
		update.width = event.get_width();
		update.height = event.get_height();
		this->updates[event.get_destination_name()].emplace(
			region->get_name(), update);
	}
}

void chomsky::TextureFingerprintManager::on_texture_copy(
	const kompvter::TextureEvent& event)
{
	remove_any(
		event.get_destination_name(),
		event.get_destination_x(), event.get_destination_y(),
		event.get_width(), event.get_height());
}

void chomsky::TextureFingerprintManager::reset()
{
	this->updates.clear();
	this->fingerprints.clear();
}

bool chomsky::TextureFingerprintManager::has(
	int texture_name, int region_name) const
{
	if (!this->use_threads)
	{
		wait();
	}

	std::lock_guard<std::mutex> lock(this->mutex);
	auto key = std::make_pair(texture_name, region_name);
	return this->fingerprints.count(key) != 0 || this->pending_fingerprints.count(key) != 0;
}

bool chomsky::TextureFingerprintManager::ready(
	int texture_name, int region_name) const
{
	if (!this->use_threads)
	{
		wait();
	}

	std::lock_guard<std::mutex> lock(this->mutex);
	auto key = std::make_pair(texture_name, region_name);
	return this->fingerprints.count(key) != 0;
}

chomsky::TextureFingerprint chomsky::TextureFingerprintManager::get(
	int texture_name, int region_name) const
{
	if (!this->use_threads)
	{
		wait();
	}

	std::lock_guard<std::mutex> lock(this->mutex);
	auto key = std::make_pair(texture_name, region_name);
	auto iter = this->fingerprints.find(key);
	if (iter != this->fingerprints.end())
	{
		return iter->second;
	}

	return TextureFingerprint();
}

void chomsky::TextureFingerprintManager::build_fingerprint(
	int texture_name, kompvter::TextureRegion region)
{
	int block_width = region.get_width() / this->num_columns;
	int block_height = region.get_height() / this->num_rows;
	if (block_width == 0)
	{
		block_width = region.get_width();
	}
	if (block_height == 0)
	{
		block_height = region.get_height();
	}

	auto key = std::make_pair(texture_name, region.get_name());
	auto fingerprint = TextureFingerprint::generate(
		region, block_width, block_height, this->color_range);
	{
		std::lock_guard<std::mutex> lock(this->mutex);
		if (this->pending_fingerprints.count(key) != 0)
		{
			this->fingerprints.emplace(key, fingerprint);
			this->pending_fingerprints.erase(key);
		}
	}
	this->update_condition.notify_one();
}

void chomsky::TextureFingerprintManager::remove_any(
	int texture_name, int x, int y, int width, int height)
{
	auto& texture_updates = this->updates[texture_name];
	auto texture = this->texture_manager->get_texture(texture_name);
	for (auto i = texture_updates.begin(); i != texture_updates.end(); /* Nothing. */)
	{
		auto region_name = i->first;
		auto& update = i->second;
		bool overlap_x = !(x >= update.x + update.width || update.x >= x + width);
		bool overlap_y = !(y >= update.y + update.height || update.y >= y + height);

		if (overlap_x && overlap_y)
		{
			std::lock_guard<std::mutex> lock(this->mutex);
			assert(texture.get_region_from_name(region_name) == nullptr);

			auto key = std::make_pair(texture_name, region_name);
			this->pending_fingerprints.erase(key);
			this->fingerprints.erase(key);

			i = texture_updates.erase(i);
		}
		else
		{
			++i;
		}
	}
}
