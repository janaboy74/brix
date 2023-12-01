#ifndef COREMAP_H_INCLUDED
#define COREMAP_H_INCLUDED

#include <set>
#include <map>
#include <memory>

using namespace std;

template <class I, class V> class coremapmodificator {
    set<I> removed;
    map<I, V> inserted;
    template<class X, class Y> friend class coremap;
public:
    void insert( const pair< I, V > item ) {
        insert( item.first, item.second );
    }
    void insert( const I item, const V val ) {
        if( removed.find( item ) != removed.end() )
            removed.erase( item );
        inserted.insert( pair<I, V>( item, val ));
    }
    void remove( I item ) {
        if( inserted.find( item ) != inserted.end() )
            inserted.erase( item );
        removed.insert( item );
    }
    void cleanup() {
        removed.clear();
        inserted.clear();
    }
};

//--------------------------------
template <class I, class V> class coremap : public map<I, V> {
//--------------------------------
    shared_ptr<coremapmodificator<I, V>> mapmodificator;
public:
    V &operator[]( const I item ) {
        static V dummy;
        if( contains( item ))
            return map<I, V>::operator[]( item );
        return dummy;
    }
    void insert( const I item, const V val ) {
        map<I, V>::insert( pair<I, V>( item, val ));
    }
    bool contains( const I item ) const {
        return this->find( item ) != this->end();
    }
    coremapmodificator<I, V> &modificator() {
        if( !mapmodificator.get() )
            mapmodificator = make_shared<coremapmodificator<I, V>>();
        return *mapmodificator.get();
    }
    void update( coremapmodificator<I, V> *modificator = nullptr ) {
        auto &ruler = modificator ? ( *modificator) : ( *mapmodificator.get() );
        if( !&ruler )
            return;
        for( auto &item : ruler.removed ) {
            map<I,V>::erase( item );
        }
        for( auto &item : ruler.inserted ) {
            map<I,V>::insert( pair<I, V>( item.first, item.second ));
        }
        if( !modificator )
            mapmodificator->cleanup();
    }
};

#endif // COREMAP_H_INCLUDED
