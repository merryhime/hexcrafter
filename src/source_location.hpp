#pragma once

#include <cstddef>

struct source_location {
    size_t line;
    size_t column;
    size_t linear_offset;
};
