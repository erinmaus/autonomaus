// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_MYSTERIMAUS_SCAN_COMPONENT_HPP
#define AUTONOMAUS_MYSTERIMAUS_SCAN_COMPONENT_HPP

#include "autonomaus/components/component.hpp"
#include "mysterimaus/scan.hpp"

namespace autonomaus
{
	class ScanComponent : public Component
	{
	public:
		ScanComponent(Autonomaus& autonomaus);
		~ScanComponent() = default;

		void reload() override;
		void show() override;
		void update() override;
		std::string get_name() const;

	private:
		bool visible = true;
		void draw();

		Scan scan;
	};
}

#endif
