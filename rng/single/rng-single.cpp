#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdarg>
#include <cinttypes>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <memory>

using namespace std;

#define TESTALGORITHM false

//-----------------------
class rng {
//-----------------------
public:
    typedef uint32_t rngType;
    rngType hash;
protected:
    void flip() {
        hash *= 0x372ce9b9;
        hash += 0xb9e92c37;
    }
public:
    rng( rngType initval = clock() ) {
        init( initval  );
    }
    ~rng() {
    }
    void init( rngType initval = clock() ) {
        hash = initval;
        flip();
    }
    void initRnd() {
        init( clock() );
    }
    rngType getRnd() {
        flip();
        return (( hash << 7 ) + ( hash >> 9 ));
    }
    rngType getRnd( rngType min, rngType max ) {
      if( min > max )
          return max;
      return (( getRnd() >> 1 ) % ( max - min )) + min;
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
class hashing : public rng {
//-----------------------
protected:
    typedef uint8_t hashType;
    shared_ptr<hashType[]> currentHash;
    size_t hashLength;
public:
    hashing( const char *hash_init = nullptr, size_t hashLength = 16 ) {
        resize( hash_init, hashLength );
    };
    void init( const char *hash_init, int count = 1 ) {
        rng::init( 0x98a410f6 );
        memset( currentHash.get(), 0xa5, hashLength * sizeof( hashType ));
        if( nullptr == hash_init ) {
            return;
        }
        addHash( hash_init, strlen( hash_init ), count );
    }
    void resize( const char *hash_init = nullptr, size_t hashLength = 16 ) {
        this->hashLength = hashLength;
        if( this->hashLength < sizeof( rngType ))
            this->hashLength = sizeof( rngType );
        currentHash = shared_ptr<hashType[]>( new hashType[ this->hashLength]);
        init( hash_init );
    }
    void genHash( const char *src, int count = 1 ) {
        genHash( src, strlen( src ), count );
    }
    void genHash( const void *src, size_t srcLength, int count = 1 ) {
        rng::init( 0x98a410f6 );
        memset( currentHash.get(), 0xa5, hashLength * sizeof( hashType ));
        if( srcLength > 1 )
            addHash(( const char *)src + 1, srcLength - 1, count );
        addHash( src, srcLength, count );
    }
    void addHash( const char *src, int count = 1 ) {
        addHash( src, strlen( src ), count );
    }
    void addHash( const void *src, size_t srcLength, int count = 1 ) {
        typedef uint8_t srcType;
        const size_t step = sizeof( rngType );
        auto *hashOut = currentHash.get(), *hashEnd = currentHash.get() + hashLength;
        const auto *chSrc = ( srcType *) src, *srcEnd = (( srcType *) src ) + srcLength;
        uint8_t state = 0;
        for(;;) {
            flip();
            if( chSrc + step > srcEnd || hashOut + step > hashEnd ) {
                int rest = srcEnd - chSrc;
                if( rest > hashEnd - hashOut  )
                    rest = hashEnd - hashOut;
                for( int pos = 0; pos < rest ; ++pos ) {
                    flip();
                    hash += *( srcType *) chSrc;
                    rngType rng = getRnd();
                    *( srcType *) hashOut ^= (( srcType* ) &rng ) [ pos ];
                    ++hashOut;
                    ++chSrc;
                }
            } else {
                hash += *( rngType *) chSrc;
                *( rngType *) hashOut ^= getRnd();
                hashOut += step;
                chSrc += step;
            }
            if( chSrc >= srcEnd ) {
                state |= 0b01;
                chSrc = ( srcType *) src;
            }
            if( hashOut >= hashEnd ) {
                state |= 0b10;
                hashOut = currentHash.get();
            }
            if( 0b11 == state ) {
                state = 0;
                if( !--count )
                    break;
            }
        }
    }
    template<class T> T singlehex( T ch, bool uppercase = false ){
        return ch < 10 ? ch + '0' : ch - 10 + ( uppercase ? 'A' : 'a' );
    }
    uint16_t toHex( uint8_t ch, bool uppercase = false ) {
        return singlehex<uint8_t>( ch >> 4, uppercase ) << 8 | singlehex<uint8_t>( ch & 0xf, uppercase );
    }
    string toHexString( bool uppercase = false, bool prefix = false ) {
        typedef uint16_t hexout;
        string result;
        size_t outlen = hashLength + ( prefix ? 1 : 0 );
        result.resize( outlen * 2 );
        hexout *out = ( hexout * ) &result.at( 0 ), *outEnd = out + outlen;
        hashType *input = currentHash.get();
        if( prefix ) {
            *out++ = ( uint16_t ) 0x7830; // : "0x" - for little endian
        }
        while( out < outEnd ) {
            *out++ = toHex( *input++, uppercase );
        }
        return result;
    }
    void printHash( bool prefix = false, bool uppercase = false ) {
        cout << toHexString( prefix, uppercase ) << endl;
    }
};

class hashTest : public hashing {
public:
    hashTest( const char *hash_init = nullptr, size_t hashLength = 24 ) : hashing( hash_init, hashLength ) {}
    void doAndPrintHash( const char * srcText, bool uppercase = false, bool prefix = false ) {
        genHash( srcText, 2 );
        cout << toHexString( uppercase, prefix ) << " : " << srcText << endl;
    }
};



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
    void reorg() { // This function is not a sorting algorithm! It just reoders the last entries limited to the given number by maintaining it's original order starting with index zero.
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
int testHash() {
//-----------------------
    cout << "-------------\n";
    cout << "hashing class\n";
    cout << "-------------\n";

    hashTest genhash( "tesó", 32 );
    genhash.printHash( true, true );
    genhash.doAndPrintHash( "a", false );
    genhash.doAndPrintHash( "aa", false );
    genhash.doAndPrintHash( "aaa", false );
    genhash.doAndPrintHash( "aaaa", false );
    genhash.doAndPrintHash( "aaaaa", false );
    genhash.doAndPrintHash( "b", true );
    genhash.doAndPrintHash( "bb", true );
    genhash.doAndPrintHash( "bbb", true );
    genhash.doAndPrintHash( "bbbb", true );
    genhash.doAndPrintHash( "bbbbb", true );
    genhash.doAndPrintHash( "ab", true );
    genhash.doAndPrintHash( "ba", true );
    genhash.doAndPrintHash( "cica", true );
    genhash.doAndPrintHash( "cicc", true );
    genhash.doAndPrintHash( "cica", true );
    genhash.doAndPrintHash( "kutyi", true );
    genhash.doAndPrintHash( "kutya", true );
    genhash.doAndPrintHash( "kutyi", true );

    loopRecord<string> record( 10 );
    rng rnd;
    vector<uint8_t> src;
    src.resize( 16 );
    genhash.resize( "cucc", 32 );

    record.resize( 10 );
    const size_t count = 100;
    for( size_t i = 0; i < count; ++i ) {
        for( auto &ch : src ) {
            ch = rnd.getRnd();
        }
        genhash.genHash(( void * ) &src.at( 0 ), src.size(), 2 );
        record.add( genhash.toHexString() );
    }

    record.reorg();
    for( int i = 0; i < record.length(); ++i ) {
        cout << record[ i ] << endl;
    }

    return 0;
}

//-----------------------
int testRng() {
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

    loopRecord<uint32_t> mathrecord( 10 );
    rng rnd;
    mathrecord.resize( 50 );
    uint32_t sum = 0;
    const size_t count = 1000;
    for( size_t i = 0; i < count; ++i ) {
        mathrecord.add( rnd.getRnd( 10, 21 ));
        sum += mathrecord.get();
    }

    mathrecord.reorg();
    for( int i = 0; i < mathrecord.length(); ++i ) {
        if( i )
            cout << ", ";
        cout << mathrecord[ i ];
    }
    cout << endl;
    cout << "avg : " << 1.f * sum / count << endl;
#if TESTALGORITHM
    cout << "------------------------\n";
    cout << "Processing test...\n";
    cout << "This may take some time.\n";
    cout << "------------------------\n";
    cout.flush();
    rng rng;
    uint32_t s = 0, hash = 0;
    uint32_t i = 0, maxuint = ~(uint32_t)0;
    bool firstloop = true;
    for( i = 0;; ++i ) {
        hash = rng.getRnd();
        if( firstloop ) {
            if( i >= maxuint ) {
                s = hash;
                i = 0;
                firstloop = false;
                cout << "first loop finished\n";
                cout.flush();
            }
        } else {
            if( s == hash || i >= maxuint ) {
                if( i >= maxuint )
                    cout << "test completed successfully\n";
                else
                    cout << "there is a problem with the algorithm with value: " << hex << setfill('0') << setw( 8 ) << i << dec << endl;
                break;
            }
        }
    }
    cout << "------------------------\n";
#endif
    return 0;
}

//-----------------------
int test() {
//-----------------------
    testHash();
    testRng();

    cout.flush();
    return 0;
}

//-----------------------
int main( int argc, char *argv[] ) {
//-----------------------
    return test();
}
//-----------------------
