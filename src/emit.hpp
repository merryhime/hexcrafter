#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <vector>

struct environment;
struct match_state;

using bytes = std::vector<std::byte>;
using emitter = std::function<bytes(const environment&, const match_state&)>;

emitter bitstring_emitter(std::string bitstring, std::map<char, std::string> mapping);
