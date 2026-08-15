#ifndef LE_BITSTRING_ENCODER_HPP
#define LE_BITSTRING_ENCODER_HPP

#include <cstdint>
#include <memory>
#include "value_objects/expr.hpp"

template<typename IMakeTrue, typename IMakeFalse, typename IMakeAbs, typename IMakeApp,
         typename IMakeVar>
struct le_bitstring_encoder {
    le_bitstring_encoder(IMakeTrue& make_true, IMakeFalse& make_false, IMakeAbs& make_abs,
                         IMakeApp& make_app, IMakeVar& make_var);
    std::shared_ptr<expr> encode_uint(uint64_t n);
private:
    std::shared_ptr<expr> make_nil();
    std::shared_ptr<expr> make_cons(std::shared_ptr<expr> head, std::shared_ptr<expr> tail);
    IMakeTrue& make_true_;
    IMakeFalse& make_false_;
    IMakeAbs& make_abs_;
    IMakeApp& make_app_;
    IMakeVar& make_var_;
};

template<typename IMakeTrue, typename IMakeFalse, typename IMakeAbs, typename IMakeApp,
         typename IMakeVar>
le_bitstring_encoder<IMakeTrue, IMakeFalse, IMakeAbs, IMakeApp, IMakeVar>::le_bitstring_encoder(
    IMakeTrue& make_true, IMakeFalse& make_false, IMakeAbs& make_abs, IMakeApp& make_app,
    IMakeVar& make_var)
    : make_true_(make_true)
    , make_false_(make_false)
    , make_abs_(make_abs)
    , make_app_(make_app)
    , make_var_(make_var) {
}

template<typename IMakeTrue, typename IMakeFalse, typename IMakeAbs, typename IMakeApp,
         typename IMakeVar>
std::shared_ptr<expr>
le_bitstring_encoder<IMakeTrue, IMakeFalse, IMakeAbs, IMakeApp, IMakeVar>::make_nil() {
    return make_abs_.make_abs(make_abs_.make_abs(make_var_.make_var(0)));
}

template<typename IMakeTrue, typename IMakeFalse, typename IMakeAbs, typename IMakeApp,
         typename IMakeVar>
std::shared_ptr<expr>
le_bitstring_encoder<IMakeTrue, IMakeFalse, IMakeAbs, IMakeApp, IMakeVar>::make_cons(
    std::shared_ptr<expr> head, std::shared_ptr<expr> tail) {
    std::shared_ptr<expr> c = make_var_.make_var(1);
    std::shared_ptr<expr> applied = make_app_.make_app(make_app_.make_app(c, head), tail);
    return make_abs_.make_abs(make_abs_.make_abs(applied));
}

template<typename IMakeTrue, typename IMakeFalse, typename IMakeAbs, typename IMakeApp,
         typename IMakeVar>
std::shared_ptr<expr>
le_bitstring_encoder<IMakeTrue, IMakeFalse, IMakeAbs, IMakeApp, IMakeVar>::encode_uint(
    uint64_t n) {
    if(n == 0)
        return make_nil();
    int msb = 63 - __builtin_clzll(n);
    std::shared_ptr<expr> result = make_nil();
    for(int i = msb; i >= 0; --i) {
        std::shared_ptr<expr> bit = ((n >> i) & 1ULL) != 0 ? make_true_.make_true()
                                                           : make_false_.make_false();
        result = make_cons(bit, result);
    }
    return result;
}

#endif
