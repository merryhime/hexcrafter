#include <catch.hpp>

#include "generator.hpp"
#include "lexer.hpp"
#include "match.hpp"
#include "peekable_generator.hpp"
#include "source_location.hpp"
#include "token.hpp"

static generator<char> char_generator_from_string(std::string s) {
    for (char c : s) {
        co_yield c;
    }
}

static std::vector<token> get_instruction(generator<token>& g) {
    std::vector<token> inst;
    for (; !g.done(); g.next()) {
        if (std::holds_alternative<semicolon_token>(g.current_value().type)) {
            g.next();
            return inst;
        }
        inst.emplace_back(g.current_value());
    }
    return inst;
}

struct environment {};

TEST_CASE("Test basic match functionality", "[match]") {
	auto s = "adc r0, lr, #3; (test)";
    auto g = lexer(peekable_generator(add_location_information(char_generator_from_string(s))));
    auto inst = get_instruction(g);

    const std::map<std::string, std::int64_t> registers {
        {"r0", 0},
        {"sp", 13},
        {"lr", 14},
        {"pc", 15},
    };

    std::vector<token_matcher> match_list {
        match_exact(symbol_token{"adc"}),
        match_enum("Rd", registers),
        match_exact(symbol_token{","}),
        match_enum("Rn", registers),
        match_exact(symbol_token{","}),
        match_exact(symbol_token{"#"}),
        match_int("const"),
    };

    match_state state;
    auto span = gsl::make_span(inst);
    for (const auto& m : match_list) {
        auto maybe_span = m(environment{}, state, span);
        if (!maybe_span) {
            FAIL();
        }
        span = *maybe_span;
    }

    REQUIRE(state.values.size() == 3);
    REQUIRE(state.values.count("Rd") == 1);
    REQUIRE(state.values["Rd"] == 0);
    REQUIRE(state.values.count("Rn") == 1);
    REQUIRE(state.values["Rn"] == 14);
    REQUIRE(state.values.count("const") == 1);
    REQUIRE(state.values["const"] == 3);
}
