#pragma once

#include <experimental/coroutine>
#include <tuple>
#include <deque>

namespace ex = std::experimental;

template <typename T>
struct peekable_generator {
    explicit peekable_generator(generator<T>&& g) : inner(std::move(g)) {}
    peekable_generator(peekable_generator&& g) : inner(std::move(g.inner)) {}

    const T& current_value() const {
        if (peek_buffer.empty()) {
            return inner.current_value();
        }
        return std::get<0>(peek_buffer.front());
    }

    bool done() const {
        if (peek_buffer.empty()) {
            return inner.done();
        }
        return std::get<1>(peek_buffer.front());
    }

    bool next() {
        if (peek_buffer.empty()) {
            return inner.next();
        }
        peek_buffer.pop_front();
        return done();
    }
    
    const T& peek_value(size_t future_index = 1) const {
        ensure_able_to_peek(future_index);
        return std::get<0>(peek_buffer[future_index]);
    }

    bool peek_done(size_t future_index = 1) const {
        ensure_able_to_peek(future_index);
        return std::get<1>(peek_buffer[future_index]);
    }

protected:
    void ensure_able_to_peek(size_t future_index) const {
        while (peek_buffer.size() <= future_index) {
            peek_buffer.emplace_back(inner.current_value(), inner.done());
            inner.next();
        }
    }

    mutable generator<T> inner;
    mutable std::deque<std::tuple<T, bool>> peek_buffer;
};
