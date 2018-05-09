// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_GPD_PARAMETER_LIST_NODE_HPP
#define CHOMSKY_GUI_GPD_PARAMETER_LIST_NODE_HPP

#include <map>
#include <string>
#include "chomsky/gui/gpd/node.hpp"

namespace chomsky { namespace gpd
{
	class ParameterListNode : public Node
	{
	public:
		ParameterListNode() = default;
		~ParameterListNode() = default;

		void parse(Tokenizer& tokenizer) override;
		void resolve(Compiler& compiler) const override;
		void compile(Compiler& compiler) const override;

		bool try_get(const std::string& parameter, int& result) const;
		bool try_get(const std::string& parameter, bool& result) const;
		bool try_get(const std::string& parameter, std::string& result) const;

	private:
		typedef std::map<std::string, std::string> ParameterList;
		ParameterList parameters;

	public:
		typedef ParameterList::const_iterator const_parameters_iterator;

		const_parameters_iterator parameters_begin() const;
		const_parameters_iterator parameters_end() const;
	};
} }

#endif
