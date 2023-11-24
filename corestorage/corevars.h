#ifndef COREVARS_H_INCLUDED
#define COREVARS_H_INCLUDED

#include <cstdint>

using namespace std;

//--------------------------------
template <class V> struct corevar {
//--------------------------------
    V var;
    corevar( const V &v = 0 ) : var( v ) {}
    V& get() {
        return var;
    }
    operator V&() {
        return var;
    }
    friend bool operator < ( const corevar& l, const corevar& r ) {
       return l.var < r.var;
    }
    friend bool operator < ( const V& l, const corevar& r ) {
       return l < r.var;
    }
    friend bool operator < ( const corevar& l, const V& r ) {
       return l.var < r;
    }
};

typedef corevar<int8_t> coreint8_t;
typedef corevar<uint8_t> coreuint8_t;
typedef corevar<int16_t> coreint16_t;
typedef corevar<uint16_t> coreuint16_t;
typedef corevar<int32_t> coreint32_t;
typedef corevar<uint32_t> coreuint32_t;
typedef corevar<int64_t> coreint64_t;
typedef corevar<uint64_t> coreuint64_t;
typedef corevar<int> coreint;
typedef corevar<long> corelong;
typedef corevar<size_t> coresize_t;
typedef corevar<float> corefloat;
typedef corevar<double> coredouble;

#endif // COREVARS_H_INCLUDED
