#ifndef POLICY_HPP
#define POLICY_HPP

#include <memory>
#include "value_objects/expr.hpp"

struct policy {
    std::shared_ptr<expr> term;
    auto operator<=>(const policy&) const = default;
};

#endif
