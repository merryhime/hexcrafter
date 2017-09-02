#include "lexer.hpp"

#include <vector>

generator<char_with_location> add_location_information(generator<char> g) {
    source_location location {1, 1, 0};
    for (; !g.done(); g.next()) {
        char ch = g.current_value();
        co_yield {ch, location};
        location.linear_offset++;
        location.column++;
        if (ch == '\n') {
            location.column = 1;
            location.line++;
        }
    }
}

generator<token> lexer(peekable_generator<char_with_location> g) {
    token current_token;

    const auto ch = [&]() -> char {
        return g.current_value().ch;
    };

    const auto consume = [&](size_t count = 1) {
        for (size_t i = 0; i < count; i++) {
            current_token.source += ch();
            g.next();
        }
    };

    const auto skip_whitespace = [&]() {
        const auto is_whitespace = [&](char c) -> bool {
            return c == 0x20 || c == 0x09 || c == 0x0D;
        };

        while (is_whitespace(ch()) && !g.done()) {
            g.next();
        }
    };

    const auto maybe = [&](const std::string& s) -> bool {
        for (size_t i = 0; i < s.size(); i++) {
            if (g.done() || g.peek_value(i).ch != s[i]) {
                return false;
            }
        }
        consume(s.size());
        return true;
    };

    const auto char_is_letter = [](char c) -> bool {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
    };

    const auto char_is_decimal_digit = [](char c) -> bool {
        return '0' <= c && c <= '9';
    };

    const auto char_is_identifier_initial = [&](char c) -> bool {
        return char_is_letter(c) || c == '_';
    };

    const auto char_is_identifier_continuant = [&](char c) -> bool {
        return char_is_identifier_initial(c) || char_is_decimal_digit(c);
    };

next_token:
    current_token = token{
        g.current_value().location,
        "",
        std::monostate{}
    };

    skip_whitespace();

    if (ch() == '\n' || g.done()) {
        current_token.type = semicolon_token{true};
        co_yield current_token;
        if (g.done()) co_return;
        goto next_token;   
    }

    if (ch() == '\n') {
        g.next();
        goto next_token;
    }

    // Single-line comment
    if (maybe("//")) {
        while (ch() != '\n' && !g.done()) {
            g.next();
        }
        goto next_token;
    }

    // Multi-line comment
    if (maybe("/*")) {
        size_t depth = 0;
        while (true) {
            if (maybe("*/")) {
                if (depth == 0) break;
                depth--;
            } else if (maybe("/*")) {
                depth++;
            } else if (g.done()) {
                current_token.type = error_token{"Expected comment to end before EOF"};
                co_yield current_token;
                co_return;
            } else {
                g.next();
            }
        }
        goto next_token;
    }

    // String Literal
    if (ch() == '"') {
        consume();
        std::string value;
        while (true) {
            char c = ch();
            consume();
            switch (c) {
            case '"':
                current_token.type = string_token{value};
                co_yield current_token;
                goto next_token;
            case '\\':
                c = ch();
                consume();
                switch (c) {
                case '"':
                    value += '"';
                    break;
                case '\\':
                    value += '\\';
                    break;
                }
                break;
            default:
                value += c;
                break;
            }
        }
    } 

    // Identifier
    if (char_is_identifier_initial(ch())) {
        consume();
        while (char_is_identifier_continuant(ch()) && !g.done()) {
            consume();
        }
        current_token.type = symbol_token{current_token.source};
        co_yield current_token;
        goto next_token;
    }

    // Numeric
    if (char_is_decimal_digit(ch())) {
        consume();
        while (char_is_decimal_digit(ch()) && !g.done()) {
            consume();
        }
        current_token.type = int_token{std::stoi(current_token.source)};
        co_yield current_token;
        goto next_token;
    }

    // Multi-character symbols
    const std::vector<std::string> special_symbols {
        "||", "&&", "<=", "!=", "==", ">=", "##"
    };
    for (const auto& s : special_symbols) {
        if (maybe(s)) {
            current_token.type = symbol_token{current_token.source};
            co_yield current_token;
            goto next_token;
        }
    }

    // Semicolon
    if (ch() == ';') {
        consume();
        current_token.type = semicolon_token{false};
        co_yield current_token;
        goto next_token;  
    }

    // Single character symbol
    consume();
    current_token.type = symbol_token{current_token.source};
    co_yield current_token;
    goto next_token;
}
