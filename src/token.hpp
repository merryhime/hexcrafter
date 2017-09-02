#pragma once

#include <string>
#include <variant>

#include "source_location.hpp"

struct string_token {
    std::string data;
};

struct int_token {
    std::int64_t data;
};

struct symbol_token {
    std::string data;
};

struct semicolon_token {
    bool elided;
};

struct error_token {
    std::string message;
};

struct token {
	using variant = std::variant<std::monostate, string_token, int_token, symbol_token, semicolon_token, error_token>;

    source_location start_location;
    std::string source;
    variant type;
};

inline bool operator==(const string_token& a, const string_token& b) {
	return a.data == b.data;
}

inline bool operator!=(const string_token& a, const string_token& b) {
	return a.data != b.data;
}

inline bool operator==(const int_token& a, const int_token& b) {
	return a.data == b.data;
}

inline bool operator!=(const int_token& a, const int_token& b) {
	return a.data != b.data;
}

inline bool operator==(const symbol_token& a, const symbol_token& b) {
	return a.data == b.data;
}

inline bool operator!=(const symbol_token& a, const symbol_token& b) {
	return a.data != b.data;
}

inline bool operator==(const semicolon_token& a, const semicolon_token& b) {
	return a.elided == b.elided;
}

inline bool operator!=(const semicolon_token& a, const semicolon_token& b) {
	return a.elided != b.elided;
}

inline bool operator==(const error_token& a, const error_token& b) {
	return a.message == b.message;
}

inline bool operator!=(const error_token& a, const error_token& b) {
	return a.message != b.message;
}
