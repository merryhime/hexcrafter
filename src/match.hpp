#pragma once

#include <cstdint>
#include <functional>
#include <gsl/span>
#include <map>
#include <optional>
#include <string>

#include "token.hpp"

struct environment;

using token_span = gsl::span<token>;

struct match_state {
    std::map<std::string, std::int64_t> values;
};

using token_matcher = std::function<std::optional<token_span>(const environment&, match_state&, token_span)>;

token_matcher match_exact(token::variant v);
token_matcher match_int(std::string name);
token_matcher match_enum(std::string name, gsl::span<const std::string> values);
token_matcher match_enum(std::string name, const std::map<std::string, std::int64_t>& values);
