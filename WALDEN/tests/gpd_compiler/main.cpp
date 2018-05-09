// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <string>
#include <fstream>
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/gpd/compiler.hpp"
#include "mantorok/resources/textureDictionary.hpp"
#include "kompvter/graphics/textureManager.hpp"

std::string read_file(const std::string& filename)
{
	std::ifstream stream(filename);
	return std::string(
		std::istreambuf_iterator<char>(stream),
		std::istreambuf_iterator<char>());
}

int main(int argc, const char* argv[])
{
	mantorok::TextureDictionary texture_dictionary;
	kompvter::TextureManager texture_manager;
	chomsky::GUIParser parser(texture_manager, texture_dictionary);
	chomsky::gpd::Compiler compiler;

	for (int i = 1; i < argc; ++i)
	{
		std::cout << "adding: " << argv[i] << std::endl;
		compiler.add_input(argv[i], read_file(argv[i]));
	}

	try
	{
		compiler.compile(parser);
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		return 1;
	}

	return 0;
}
