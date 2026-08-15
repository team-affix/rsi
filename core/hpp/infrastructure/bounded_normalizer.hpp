#ifndef BOUNDED_NORMALIZER_HPP
#define BOUNDED_NORMALIZER_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include "value_objects/expr.hpp"

template<typename IMakeRuntime>
struct bounded_normalizer {
    bounded_normalizer(IMakeRuntime& make_runtime, uint64_t max_steps, uint64_t max_bytes);
    std::optional<std::shared_ptr<expr>> normalize(std::shared_ptr<expr> term);
private:
    IMakeRuntime& make_runtime_;
    uint64_t max_steps_;
    uint64_t max_bytes_;
};

template<typename IMakeRuntime>
bounded_normalizer<IMakeRuntime>::bounded_normalizer(IMakeRuntime& make_runtime,
                                                    uint64_t max_steps, uint64_t max_bytes)
    : make_runtime_(make_runtime)
    , max_steps_(max_steps)
    , max_bytes_(max_bytes) {
}

template<typename IMakeRuntime>
std::optional<std::shared_ptr<expr>>
bounded_normalizer<IMakeRuntime>::normalize(std::shared_ptr<expr> term) {
    auto&& rt = make_runtime_.make(term);
    for (uint64_t steps = 0;
        (
            !rt.done() &&
            steps < max_steps_ &&
            rt.space_usage() < max_bytes_
        ); ++steps)
        rt.step();
    return rt.done() ? std::make_optional(rt.output()) : std::nullopt;
}

#endif
