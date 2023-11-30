#ifndef COREVECTOR_H_INCLUDED
#define COREVECTOR_H_INCLUDED

#include <vector>

using namespace std;

//--------------------------------
template <class V> struct corevector : public vector<V> {
//--------------------------------
    corevector( const size_t s, const V v ) {
        vector<V>::resize( s );
        for( auto &var : *this )
            var = v;
    }
    V &operator[]( size_t index ) {
        static V dummy;
        if( index < this->size() )
            return vector<V>::operator[]( index );
        return dummy;
    }
};

#endif // COREVECTOR_H_INCLUDED
