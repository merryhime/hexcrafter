#include <cstdio>
#include <cstdint>
#include <gsl/span>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "generator.hpp"
#include "lexer.hpp"
#include "match.hpp"
#include "peekable_generator.hpp"
#include "source_location.hpp"
#include "token.hpp"

generator<char> char_generator_from_string(std::string s) {
    for (char c : s) {
        co_yield c;
    }
}

void print_token(const token& t) {
    printf("(%zu:%zu) %s - ", t.start_location.line, t.start_location.column, t.source.c_str());
    std::visit([](const auto& type){
        using T = std::decay_t<decltype(type)>;
        if constexpr (std::is_same_v<T, string_token>)
            printf("string : %s\n", type.data.c_str());
        if constexpr (std::is_same_v<T, int_token>)
            printf("int : %llu\n", type.data);
        if constexpr (std::is_same_v<T, symbol_token>)
            printf("symbol : %s\n", type.data.c_str());
        if constexpr (std::is_same_v<T, semicolon_token>)
            printf("semicolon : %s\n", type.elided ? "true" : "false");
        if constexpr (std::is_same_v<T, error_token>)
            printf("error : %s\n", type.message.c_str());
        if constexpr (std::is_same_v<T, std::monostate>)
            printf("monostate\n");
    }, t.type);
}

std::vector<token> get_instruction(generator<token>& g) {
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

int main(int, char** argv) {
    auto g = lexer(peekable_generator(add_location_information(char_generator_from_string(argv[1]))));
    auto inst = get_instruction(g);
    for (const token& t : inst) {
        print_token(t);
    }
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
        if (!maybe_span)
            printf("oops.\n");
        span = *maybe_span;
    }

    for (const auto& p : state.values) {
        printf("%s : %lli\n", p.first.c_str(), p.second);
    }
}
