#ifndef CORESET_H_INCLUDED
#define CORESET_H_INCLUDED

#include <set>

using namespace std;

//--------------------------------
template <class V> struct coreset : public set<V> {
//--------------------------------
    V &operator[]( const V val ) {
        static V dummy;
        if( contains( val ))
            return set< V>::operator[]( val );
        return dummy;
    }
    bool contains( const V &val ) const {
        return set<V>::find( val ) != this->end();
    }
};

#endif // CORESET_H_INCLUDED
