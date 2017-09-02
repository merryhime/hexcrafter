#pragma once

#include <experimental/coroutine>

namespace ex = std::experimental;

template <typename T>
struct generator {
    struct promise_type {
        bool done = false;
        T current_value;

        auto yield_value(T value) {
            current_value = std::move(value);
            return ex::suspend_always{};
        }

        auto initial_suspend() { 
            return ex::suspend_never{};
        }

        auto final_suspend() {
            done = true;
            return ex::suspend_always{};
        }

        generator get_return_object() { 
            return generator{this};
        }

        void unhandled_exception() {
            std::terminate();
        }

        void return_void() {}
    };

    const T& current_value() const {
        return p.promise().current_value;
    }

    bool done() const {
        return p.promise().done;
    }

    bool next() {
        if (!done()) {
            p.resume();
        }
        return done();
    }

    generator(generator&& g) : p(g.p) {
        g.p = nullptr;
    }

    ~generator() {
        if (p) {
            p.destroy();
        }
    }

protected:
    explicit generator(promise_type *p)
        : p(ex::coroutine_handle<promise_type>::from_promise(*p)) {}

    ex::coroutine_handle<promise_type> p;
};
