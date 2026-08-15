#include "infrastructure/uniform_rng.hpp"

#include "debug_assert.hpp"

uniform_rng::uniform_rng(uint64_t seed)
    : rng_(seed) {
}

uint64_t uniform_rng::sample_uniform(uint64_t n) {
    DEBUG_ASSERT(n > 0);
    std::uniform_int_distribution<uint64_t> dist(0, n - 1);
    return dist(rng_);
}
