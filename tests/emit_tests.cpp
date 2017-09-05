#include <catch.hpp>

#include "emit.hpp"
#include "match.hpp"

struct environment {};

TEST_CASE("Test basic emit functionality", "[emit]") {
    auto emitter = bitstring_emitter("cccc0000101Snnnnddddvvvvvrr0mmmm", {
        {'c', "cond"},
        {'S', "S"},
        {'n', "Rn"},
        {'d', "Rd"},
        {'v', "imm5"},
        {'r', "type"},
        {'m', "Rm"},
    });

    match_state state {
        {
            {"cond", 0xE},
            {"S", 0},
            {"Rn", 1},
            {"Rd", 2},
            {"imm5", 0},
            {"type", 0},
            {"Rm", 15},
        }
    };

    auto b = emitter(environment{}, state);
    REQUIRE(b[3] == std::byte{0xE0});
    REQUIRE(b[2] == std::byte{0xA1});
    REQUIRE(b[1] == std::byte{0x20});
    REQUIRE(b[0] == std::byte{0x0F});
}
