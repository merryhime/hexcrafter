#include "match.hpp"

#include <algorithm>

#include "container_util.hpp"

token_matcher match_exact(token::variant v) {
    return [v](const environment&, match_state&, token_span in) -> std::optional<token_span> {
        if (in.empty()) return std::nullopt;
        if (in[0].type != v) return std::nullopt;
        return in.subspan<1>();
    };
}

token_matcher match_int(std::string name) {
    return [name](const environment&, match_state& state, token_span in) -> std::optional<token_span> {
        assert(state.values.count(name) == 0);

        if (in.empty()) return std::nullopt;
        if (!std::holds_alternative<int_token>(in[0].type)) return std::nullopt;

        state.values[name] = std::get<int_token>(in[0].type).data;

        return in.subspan<1>();
    };
}

token_matcher match_enum(std::string name, gsl::span<std::string> values) {
    return [name, values](const environment&, match_state& state, token_span in) -> std::optional<token_span> {
        assert(state.values.count(name) == 0);

        if (in.empty()) return std::nullopt;
        if (!std::holds_alternative<symbol_token>(in[0].type)) return std::nullopt;

        auto index = index_of(values, std::get<symbol_token>(in[0].type).data);
        if (!index) return std::nullopt;
        state.values[name] = static_cast<std::int64_t>(*index);

        return in.subspan<1>();
    };
}

token_matcher match_enum(std::string name, const std::map<std::string, std::int64_t>& values) {
    return [name, values](const environment&, match_state& state, token_span in) -> std::optional<token_span> {
        assert(state.values.count(name) == 0);

        if (in.empty()) return std::nullopt;
        if (!std::holds_alternative<symbol_token>(in[0].type)) return std::nullopt;

        auto iter = values.find(std::get<symbol_token>(in[0].type).data);
        if (iter == values.end()) return std::nullopt;
        state.values[name] = iter->second;

        return in.subspan<1>();
    };
}

