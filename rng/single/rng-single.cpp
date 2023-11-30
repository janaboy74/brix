#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdarg>
#include <cinttypes>
#include <vector>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

//-----------------------
class rng {
//-----------------------
    uint32_t hash;
    void flip() {
        hash *= 0x372ce9b9;
        hash += 0xb9e92c37;
    }
public:
    rng( int32_t initval = clock() ) {
        init( initval  );
    }
    ~rng() {
    }
    void init( int32_t initval = clock() ) {
        hash = initval;
        flip();
    }
    void initRnd() {
        init( clock() );
    }
    int32_t getRnd() {
        flip();
        return (( hash << 7 ) ^ ( hash >> 9 ))  >> 1;
    }
    int32_t getRnd( int32_t min, int32_t max ) {
      if( min > max )
          return max;
      return ( getRnd() % ( max - min )) + min;
    }
};

//-----------------------
int32_t get_rnd( int32_t min = 0, int32_t max = 0 ) {
//-----------------------
    static uint32_t hash = ( min == max ) ? clock() : min;
    hash *= 0x372ce9b9;
    hash += 0xb9e92c37;
    if( min >= max )
        return max;
    return min + (( hash << 7 ) ^ ( hash >> 9 )  >> 1 ) % ( max - min );
}

//-----------------------
void gen_hash( const void *src, size_t srcLength, uint8_t *hashOutput, size_t hashOutputInChar ) {
//-----------------------
    typedef uint8_t hashType;
    typedef uint32_t srcType;
    hashType hash = ( hashType ) 0x98541076;
    size_t restlength = hashOutputInChar;
    memset( hashOutput, 0, hashOutputInChar );
    const size_t step = sizeof( hashType );
    auto *hashOut = hashOutput, *hashEnd = hashOutput + hashOutputInChar;
    const auto *chSrc = ( srcType *) src, *srcEnd = (( srcType *) src ) + srcLength;
    for( ;chSrc < srcEnd; ) {
        hash *= 0x372ce9b9;
        hash += 0xb9e92c37;
        if( hashOutputInChar < step ) {
            for( int pos = 0; pos < step; ++pos )
                hash += *( srcType *) chSrc;
        } else {
            hash += *( hashType *) chSrc;
        }
        *hashOut ^= hash;
        hashOut += step;
        if( hashOut >= hashEnd )
            hashOut = hashOutput;
        chSrc += step;
    }
}

void print_hash( const uint8_t *hashOutput, size_t hashOutputInChar ) {
    cout << "0x";
    auto hashPtr = hashOutput, hasEnd = hashOutput + hashOutputInChar;
    for( ;hashPtr < hasEnd; ++hashPtr) {
        cout << hex << setfill('0') << setw( 2 ) << static_cast<int>( *hashPtr ) << dec;
    }
    cout << endl;
}

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
        this->size = size;
        reset();
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
    vector<char> hashOut;
    hashOut.resize( 32 );

    record.resize( 99 );
    const size_t count = 1000;
    for( size_t i = 0; i < count; ++i ) {
        record.add( rnd.getRnd( 10, 21 ));
        sum += record.get();
    }

    gen_hash(( void* ) &record.get( 0 ), sizeof( record.get() ) * record.length(), ( uint8_t *) hashOut.data(), hashOut.size() );
    cout << "hash:"; print_hash(( uint8_t *) hashOut.data(), hashOut.size() );

    record.reorg();
    for( int i = 0; i < record.length(); ++i ) {
        if( i )
            cout << ", ";
        cout << record[ i ];
    }
    cout << endl;
    gen_hash(( void* ) &record.get( 0 ), sizeof( record.get() ) * record.length(), ( uint8_t *) hashOut.data(), hashOut.size() );
    cout << "hash:";print_hash(( uint8_t *) hashOut.data(), hashOut.size() );
    gen_hash(( void* ) &record.get( 0 ), sizeof( record.get() ) * record.length(), ( uint8_t *) hashOut.data(), hashOut.size() );
    cout << "hash:";print_hash(( uint8_t *) hashOut.data(), hashOut.size() );
    cout << "avg : " << 1.f * sum / count << endl;

    cout.flush();
}

//-----------------------
int main( int argc, char *argv[] ) {
//-----------------------
    test();
    return 0;
}
//-----------------------
