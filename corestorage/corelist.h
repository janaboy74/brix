#ifndef CORELIST_H_INCLUDED
#define CORELIST_H_INCLUDED

#include <list>

using namespace std;

//--------------------------------
template <class V> struct corelist : public list<V> {
//--------------------------------
    V &operator[]( size_t index ) {
        static V dummy;
        if( index < this->size() )
            return list<V>::operator[]( index );
        return dummy;
    }
};

#endif // CORELIST_H_INCLUDED
