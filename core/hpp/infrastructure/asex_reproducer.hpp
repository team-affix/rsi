#ifndef ASEX_REPRODUCER_HPP
#define ASEX_REPRODUCER_HPP

#include <cstddef>
#include <vector>
#include "value_objects/asex_agent.hpp"
#include "value_objects/asex_reproduction_context.hpp"

template<typename IGenerateChild>
struct asex_reproducer {
    asex_reproducer(IGenerateChild& generate_child, std::size_t r, std::size_t max_retries);
    std::vector<asex_agent> reproduce(const asex_reproduction_context& ctx);
private:
    IGenerateChild& generate_child_;
    std::size_t r_;
    std::size_t max_retries_;
};

template<typename IGenerateChild>
asex_reproducer<IGenerateChild>::asex_reproducer(IGenerateChild& generate_child, std::size_t r,
                                                std::size_t max_retries)
    : generate_child_(generate_child)
    , r_(r)
    , max_retries_(max_retries) {
}

template<typename IGenerateChild>
std::vector<asex_agent>
asex_reproducer<IGenerateChild>::reproduce(const asex_reproduction_context& ctx) {
    std::vector<asex_agent> children;
    std::size_t fails = 0;
    while(children.size() < r_ && fails < max_retries_) {
        auto child = generate_child_.generate(ctx.agent.rec);
        if(!child.has_value()) {
            ++fails;
            continue;
        }
        children.push_back(*child);
    }
    return children;
}

#endif
