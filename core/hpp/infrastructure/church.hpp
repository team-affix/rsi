#ifndef CHURCH_HPP
#define CHURCH_HPP

#include <memory>
#include "value_objects/expr.hpp"

template<typename IMakeAbs, typename IMakeVar>
struct church {
    church(IMakeAbs& make_abs, IMakeVar& make_var);
    std::shared_ptr<expr> make_true();
    std::shared_ptr<expr> make_false();
private:
    IMakeAbs& make_abs_;
    IMakeVar& make_var_;
};

template<typename IMakeAbs, typename IMakeVar>
church<IMakeAbs, IMakeVar>::church(IMakeAbs& make_abs, IMakeVar& make_var)
    : make_abs_(make_abs)
    , make_var_(make_var) {
}

template<typename IMakeAbs, typename IMakeVar>
std::shared_ptr<expr> church<IMakeAbs, IMakeVar>::make_true() {
    return make_abs_.make_abs(make_abs_.make_abs(make_var_.make_var(1)));
}

template<typename IMakeAbs, typename IMakeVar>
std::shared_ptr<expr> church<IMakeAbs, IMakeVar>::make_false() {
    return make_abs_.make_abs(make_abs_.make_abs(make_var_.make_var(0)));
}

#endif
