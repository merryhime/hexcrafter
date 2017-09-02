#pragma once

#include "generator.hpp"
#include "peekable_generator.hpp"
#include "source_location.hpp"
#include "token.hpp"

struct char_with_location {
	char ch;
	source_location location;
};

generator<char_with_location> add_location_information(generator<char> g);
generator<token> lexer(peekable_generator<char_with_location> g);
