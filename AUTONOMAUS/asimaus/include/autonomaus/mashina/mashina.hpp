// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_MASHINA_MASHINA_HPP
#define AUTONOMAUS_MASHINA_MASHINA_HPP

#include <glm/glm.hpp>
#include "autonomaus/core/autonomaus.hpp"
#include "bmashina/bmashina.hpp"
#include "bmashina/builder/builder.hpp"
#include "bmashina/debug/debug.hpp"
#include "bmashina/primitives/primitives.hpp"

namespace autonomaus { namespace mashina
{
	// Nodes.
	typedef bmashina::BasicNode<Autonomaus> Node;
	typedef bmashina::BasicComposite<Autonomaus> Composite;
	typedef bmashina::BasicDecorator<Autonomaus> Decorator;
	using Status = bmashina::Status;

	// Primitives.
	typedef bmashina::Invert<Autonomaus> Invert;
	typedef bmashina::Failure<Autonomaus> Failure;
	typedef bmashina::Sequence<Autonomaus> Sequence;
	typedef bmashina::Selector<Autonomaus> Selector;
	typedef bmashina::Success<Autonomaus> Success;

	// State.
	typedef bmashina::BasicState<Autonomaus> State;
	template <typename V>
	using Property = bmashina::Property<V>;
	template <typename V>
	using Reference = bmashina::Reference<V>;
	template <typename V>
	using Local = bmashina::Local<V>;

	// Trees.
	typedef bmashina::BasicTree<Autonomaus> Tree;
	typedef bmashina::BasicChannel<Autonomaus>::Type Channel;
	typedef bmashina::NativeTreeBuilder TreeBuilder;
	typedef bmashina::BasicExecutor<Autonomaus> Executor;

	// Debug.
	typedef bmashina::BasicContext<Autonomaus> DebugContext;
	using DebugBreak = bmashina::Break;
} }

#include <glm/gtx/string_cast.hpp>
#include "thoreau/tileCoordinate.hpp"
#include "chomsky/gui/guiContent.hpp"

namespace bmashina
{
	template <typename M>
	struct ToString<M, std::vector<std::string>*>
	{
		inline static typename String<M>::Type get(M& mashina, const std::vector<std::string>* const& value)
		{
			std::stringstream stream;
			for (auto& i: *value)
			{
				stream << "- " << i << std::endl;
			}
			return stream.str();
		}
	};

	template <typename M, typename V>
	struct ToString<M, typename glm::tvec2<V>>
	{
		inline static typename String<M>::Type get(M& mashina, const glm::tvec2<V>& value)
		{
			return glm::to_string(value);
		}
	};

	template <typename M, typename V>
	struct ToString<M, typename glm::tvec3<V>>
	{
		inline static typename String<M>::Type get(M& mashina, const glm::tvec3<V>& value)
		{
			return glm::to_string(value);
		}
	};

	template <typename M, typename V>
	struct ToString<M, typename glm::tvec4<V>>
	{
		inline static typename String<M>::Type get(M& mashina, const glm::tvec4<V>& value)
		{
			return glm::to_string(value);
		}
	};

	template <typename M>
	struct ToString<M, thoreau::TileCoordinate>
	{
		inline static typename String<M>::Type get(M& mashina, const thoreau::TileCoordinate& value)
		{
			std::stringstream stream;
			stream << "(" << value.x << ", " << value.y << "; " << value.layer << ")";
			return stream.str();
		}
	};

	template <typename M>
	struct ToString<M, chomsky::GUIContent>
	{
		inline static typename String<M>::Type get(M& mashina, const chomsky::GUIContent& value)
		{
			std::stringstream stream;

			auto name = value.get_name();
			auto widget = value.get_widget_class();
			auto num_elements = value.count();
			auto x = value.get_x();
			auto y = value.get_y();
			auto width = value.get_width();
			auto height = value.get_height();
			if (name.empty())
			{
				name = "(anon)";
			}
			if (widget.empty())
			{
				widget = "property";
			}

			stream << name << ":" << widget
				<< "[" << num_elements << "]"
				<< "@" << x << "," << y
				<< "/" << width << "x" << height;

			return stream.str();
		}
	};

	// kvlc::Query::const_iterator::value_type
	template <typename M>
	struct ToString<M, std::pair<int, glm::vec3>>
	{
		inline static typename String<M>::Type get(M& mashina, const std::pair<int, glm::vec3>& value)
		{
			auto tag = value.first;
			auto position = value.second;
			std::stringstream stream;
			stream << tag << " " << glm::to_string(position);
			return stream.str();
		}
	};
}

// For convenience.
#include "autonomaus/core/config.hpp"

#endif
