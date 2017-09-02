#pragma once

#include <cstddef>
#include <optional>

template <typename Container, typename T>
std::optional<size_t> index_of(const Container& c, const T& t) {
    using std::begin;
    using std::end;

    auto pos = std::find(begin(c), end(c), t);
    if (pos == end(c)) return std::nullopt;
    return pos - begin(c);
}
