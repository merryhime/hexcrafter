#include "emit.hpp"

#include <bitset>
#include <climits>
#include <cstddef>
#include <cstdint>

#include "match.hpp"

template <typename T>
static size_t bit_size() {
    return sizeof(T) * CHAR_BIT;
}

template <typename T>
static size_t bit_count(T value) {
    return std::bitset<bit_size<T>()>(value).count();
}

template <typename Integral>
static Integral deposit_bits(Integral x, Integral mask) {
    Integral res = 0;
    for (Integral bb = 1; mask != 0; bb += bb) {
        if (x & bb) { res |= mask & (-mask); }
        mask &= (mask - 1);
    }
    return res;
}

static bytes to_bytes(std::uint64_t value, size_t num_bytes) {
    bytes result(num_bytes);
    for (size_t i = 0; i < num_bytes; ++i) {
        result[i] = static_cast<std::byte>(value >> (i * 8));
    }
    return result;
}

emitter bitstring_emitter(std::string bitstring, std::map<char, std::string> mapping) {
    assert(bitstring.length() % 8 == 0);
    assert(bitstring.length() <= 64);

    const size_t byte_count = bitstring.length() / 8;

    std::uint64_t base_bits = 0;
    std::map<char, std::uint64_t> part_bits;
    for (size_t i = 0; i < bitstring.length(); i++) {
        const char ch = bitstring[i];
        const size_t bit_pos = bitstring.length() - i - 1;
        const std::uint64_t bit = 1 << bit_pos;

        switch (ch) {
        case '0':
        case '-':
            // Do nothing
            break;
        case '1':
            base_bits |= bit;
            break;
        default:
            assert(mapping.count(ch) == 1);
            part_bits[ch] |= bit;
            break;
        }
    }

    return [=](const environment&, const match_state& state) -> bytes {
        std::uint64_t result = base_bits;
        for (const auto& p : part_bits) {
            const std::string& name = mapping.at(p.first);
            const std::uint64_t mask = p.second;

            assert(state.values.count(name) == 1);
            assert(state.values.at(name) < (1 << bit_count(mask)));
            result |= deposit_bits(static_cast<std::uint64_t>(state.values.at(name)), mask);
        }
        return to_bytes(result, byte_count);
    };
}
