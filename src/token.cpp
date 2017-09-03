#include "token.hpp"

#include <type_traits>

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