#include "infrastructure/lc_runtime_maker.hpp"

lc_runtime_maker::lc_runtime_maker(rc_pool<expr>& out_nodes)
    : out_nodes_(out_nodes) {
}

runtime lc_runtime_maker::make(std::shared_ptr<expr> term) {
    return runtime(std::move(term), out_nodes_);
}
