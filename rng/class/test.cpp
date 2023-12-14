#include "rng.h"
#include <iostream>

using namespace std;

//--------------------------------
template <typename T> class loopRecord {
//--------------------------------
private:
    size_t size;
    size_t filled;
    size_t pos;
    T *array;
public:
    loopRecord( size_t size = 0 ) : filled( 0 ), size( size ), pos( 0 ), array( nullptr ) {
        if( size )
            resize( size );
    };
    ~loopRecord() {
        delete[] array;
    }
    void resize( size_t size ) {
        if( array )
            delete[] array;
        array = new T[ size ];
        pos = 0;
    }
    void reset() {
        pos = 0;
        filled = 0;
    }
    void add( T item ) {
        if( pos >= size )
            pos = 0;
        array[ pos ] = item;
        ++pos;
        if( filled != size ) {
            ++filled;
            if( filled > size )
                filled = size;
        }
    }
    T &operator []( size_t itemID ) {
        return get( itemID );
    }
    T &get() {
        auto last = pos; --last;
        return array[ last ];
    }
    T &get( size_t itemID ) {
        if( itemID < size )
            return array[ itemID ];
        static T failure;
        return failure;
    }
    int length() {
        return filled;
    }
    void reorg() {
        int cur = 0, delta = filled - pos;
        if( pos >= filled )
            return;
        for( ;; ) {
            swap( array[ cur++ ], array[ pos++ ]);
            if( pos >= filled ) {
                if( delta >= pos - cur )
                    delta %= pos - cur;
                if( 0 == delta )
                    break;
                pos -= delta;
            }
        }
    }
};

//-----------------------
void test() {
//-----------------------
    cout << "-------------\n";
    cout << "hashing class\n";
    cout << "-------------\n";

    hashing genhash( "tesó", 32 );
    genhash.printHash( true, true );
    genhash.genHash( "tesó" );
    genhash.printHash( false );
    genhash.genHash( "alap" );
    genhash.printHash( false );
    genhash.genHash( "power" );
    genhash.printHash( false );
    genhash.genHash( "pow" );
    genhash.printHash( false );

    cout << "------\n";
    cout << "rnd fx\n";
    cout << "------\n";
    ushort intype = (( ushort ) clock() ) % 3;
    switch( intype ) {
    case 2: {
            int32_t initval = -(( uint32_t ) clock());
            get_rnd( initval );
            cout << "inited with negative: " << initval <<  "\n";
            break;
        }
    case 1: {
            int32_t initval = +(( uint32_t ) clock());
            get_rnd( initval );
            cout << "inited with positive: " << initval <<  "\n";
            break;
        }
    default:
        get_rnd( 0 );
        cout << "inited with clock\n";
        break;
    }

    for( size_t i = 0; i < 3; ++i ) {
        if( i )
            cout <<", ";
        cout << get_rnd( 10, 40 );
    }
    cout << endl;

    cout << "---------\n";
    cout << "rnd class\n";
    cout << "---------\n";

    rng rnd;
    uint32_t sum( 0 );
    loopRecord<uint32_t> record( 10 );

    const size_t count = 1000;
    for( size_t i = 0; i < count; ++i ) {
        record.add( rnd.getRnd( 10, 21 ));
        sum += record.get();
    }
    record.reorg();
    for( int i = 0; i < record.length(); ++i ) {
        if( i )
            cout << ", ";
        cout << record[ i ];
    }
    cout << "\n avg : " << 1.f * sum / count << endl;

    cout.flush();
}

//-----------------------
int main( int argc, char *argv[] ) {
//-----------------------
    test();
    return 0;
}
//-----------------------
