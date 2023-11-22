#ifndef COREMAP_H_INCLUDED
#define COREMAP_H_INCLUDED

#include <map>

using namespace std;

//--------------------------------
template <class I, class V> struct coremap : public map<I, V> {
//--------------------------------
    V &operator[]( I item ) {
        static V dummy;
        if( contains( item ))
            return map<I, V>::operator[]( item );
        return dummy;
    }
    void insert( I item, V val ) {
        map<I, V>::insert( pair<I, V>( item, val ));
    }
    bool contains( I item ) {
        return this->find( item ) != this->end();
    }
};

#endif // COREMAP_H_INCLUDED
