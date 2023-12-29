#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdarg>
#include <cinttypes>
#include <vector>
#include <cmath>
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
                if( rest > hashEnd - hashOut )
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

//-----------------------
class rngparams : public rng {
//-----------------------
public:
    rngType mul;
    rngType add;
    void gen( size_t minDigitCount = 1, size_t dontMatchLastNDigit = 0 ) {
        init();
        short dontMatchLastBits = dontMatchLastNDigit * 4;
        for(;;) {
            for( size_t i = 0;; ) {
                size_t max = getRnd( 1, 4 );
                if( !i && getRnd( 0, 1 ))
                    max = 0;
                for( size_t s = 0; s < max; ++s ) {
                    mul = mul << 1 | 1; ++i;
                    if( i >= sizeof( rngType ) * 8 )
                        break;
                }
                if( i >= sizeof( rngType ) * 8 )
                    break;
                max = getRnd( 1, 4 );
                for( size_t s = 0; s < max; ++s ) {
                    mul = mul << 1 | 0; ++i;
                    if( i >= sizeof( rngType ) * 8 )
                        break;
                }
                if( i >= sizeof( rngType ) * 8 )
                    break;
            }
            mul = mul << 2;
            mul = ( mul & ~3 ) | 1 ;
            bool fail = false;
            short found = 0;
            size_t fcount = 0;
            auto test = mul;
            for( size_t rotate = 0; rotate < sizeof( rngType ) * 8; rotate += 4 ) {
                uint8_t cur = ( test >> rotate ) & 0xf;
                found |= 1 << cur;
                for( size_t cnt = 4; cnt < dontMatchLastBits && cnt <= rotate ; cnt += 4 ) {
                    uint8_t act = test >> ( rotate - cnt ) &0xf;
                    if( act == cur ) {
                        fail = true;
                        break;
                    }
                }
                if( fail )
                    break;
            }
            if( fail )
                continue;
            for( size_t rotate = 0; rotate < sizeof( rngType ) * 8; ++rotate ) {
                if(( found >> rotate ) & 1 ) {
                    ++fcount ;
                }
            }
            if( fcount < minDigitCount )
                continue;
            break;
        }
        for(;;) {
            init();
            add = 0;
            for( size_t i = 0;; ) {
                size_t max = getRnd( 1, 4 );
                if( !i && getRnd( 0, 1 ))
                    max = 0;
                for( size_t s = 0; s < max; ++s ) {
                    add = add << 1 | 1; ++i;
                    if( i >= sizeof( rngType ) * 8 )
                        break;
                }
                if( i >= sizeof( rngType ) * 8 )
                    break;
                max = getRnd( 1, 4 );
                for( size_t s = 0; s < max; ++s ) {
                    add = add << 1 | 0; ++i;
                    if( i >= sizeof( rngType ) * 8 )
                        break;
                }
                if( i >= sizeof( rngType ) * 8 )
                    break;
            }
            add = add | 1;
            bool fail = false;
            short found = 0;
            size_t fcount = 0;
            auto test = mul;
            test = add;
            for( size_t rotate = 0; rotate < sizeof( rngType ) * 8; rotate += 4 ) {
                uint8_t cur = ( test >> rotate ) & 0xf;
                found |= 1 << cur;
                for( size_t cnt = 4; cnt < dontMatchLastBits && cnt <= rotate ; cnt += 4 ) {
                    uint8_t act = test >> ( rotate - cnt ) &0xf;
                    if( act == cur ) {
                        fail = true;
                        break;
                    }
                }
                if( fail )
                    break;
            }
            if( fail )
                continue;
            fcount = 0; for( size_t rotate = 0; rotate < sizeof( rngType ) * 8; ++rotate ) {
                if(( found >> rotate ) & 1 ) {
                    ++fcount ;
                }
            }
            if( fcount < minDigitCount )
                continue;
            break;
        }
    }
    template<class T> static T singlehex( T ch, bool uppercase = false ){
        return ch < 10 ? ch + '0' : ch - 10 + ( uppercase ? 'A' : 'a' );
    }
    static void printHex( rngType input, bool uppercase = false, bool prefix = false ) {
        if( prefix )
            cout << "0x";
        size_t max = sizeof( rngType );
        uint8_t *pch = (( uint8_t * ) &input ) + max - 1;
        for( size_t i = 0; i < sizeof( rngType ); ++i, --pch ) {
            cout << singlehex<char>( *pch >> 4, uppercase );
            cout << singlehex<char>( *pch & 0xf, uppercase );
        }
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
class shuffler {
    //-----------------------
public:
    typedef uint8_t shufflerType;
    shufflerType hash;
    shufflerType mul;
    shufflerType add;
protected:
    void flip() {
        hash *= mul;
        hash += add;
    }
public:
    shuffler( const shufflerType begin = 0, const shufflerType mul = 0xc9, const shufflerType add = 0x7a ) : hash( begin ), mul( mul ), add( add ) {
        //reInit( begin, mul , add );
    }
    ~shuffler() {
    }
    void reInit( const shufflerType begin = 0, const shufflerType mul = 0xc9, const shufflerType add = 0x7a ) {
        hash = begin;
        this->mul = ( mul & ~3 ) | 1 ;
        this->add = add | 1;
    }
    shufflerType getNext() {
        flip();
        return hash;
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
    rng::rngType s = 0, hash = 0;
    rng::rngType i = 0, maxrng = ~(rng::rngType)0;
    bool firstloop = true;
    for( i = 0;; ++i ) {
        hash = rng.getRnd();
        if( firstloop ) {
            if( i >= maxrng ) {
                s = hash;
                i = 0;
                firstloop = false;
                cout << "first loop finished\n";
                cout.flush();
            }
        } else {
            if( s == hash || i >= maxrng ) {
                if( i >= maxrng )
                    cout << "test completed successfully\n";
                else
                    cout << "there is a problem with the algorithm with value: " << hex << setfill('0') << setw( 8 ) << i << dec << endl;
                break;
            }
        }
    }
    cout << "------------------------\n";
#endif
    cout << "----------------------\n";
    cout << "rngparams helper class\n";
    cout << "----------------------\n";
    rngparams rngprms;
    for( int z = 0; z < 10; ++z ) {
        rngprms.gen( 6, 3 );
        cout << "mul: "; rngprms.printHex( rngprms.mul ); cout << ", add: "; rngprms.printHex( rngprms.add ); cout << "\n";
    }

    return 0;
}

int testShuffler() {
    cout << "--------------\n";
    cout << "shuffler class\n";
    cout << "--------------\n";

    shuffler shflr;
    shuffler::shufflerType maxshuffler = ~(shuffler::shufflerType) 0;
    uint32_t success = 0;
    uint32_t problems = 0;

    for( uint16_t add = 1; add <= maxshuffler; add += 2 ) {
        for( uint16_t mul = 1; mul <= maxshuffler; mul += 4 ) {
            shflr.reInit( 0, mul, add );
            shuffler::shufflerType s = 0, hash = 0;
            bool firstloop = true;
            for( shuffler::shufflerType i = 0;; ++i ) {
                hash = shflr.getNext();
                if( firstloop ) {
                    if( i >= maxshuffler ) {
                        s = hash;
                        i = 0;
                        firstloop = false;
                        cout.flush();
                    }
                } else {
                    if( s == hash || i >= maxshuffler ) {
                        if( i >= maxshuffler )
                            ++success;
                        else
                            ++problems;
                        break;
                    }
                }
            }
        }
    }
    cout << "shuffler test completed with: " << success << " success & " << problems << " problems\n";
    cout << "shuffler example:";
    shflr.reInit( get_rnd( 0, 255 ), get_rnd( 0, 255 ), get_rnd( 0, 255 ));
    for( uint16_t i = 0; i <= maxshuffler; ++i ) {
        if( i )
            cout << ", ";
        if( !( i & 15 ))
            cout << "\n";
        cout << hex << setfill('0') << setw( 2 ) << ( uint16_t ) shflr.getNext() << dec;
    }
    cout << "\n";

    return 0;
}

uint32_t nextPrime( uint32_t value) {
    if( value > 2 ) {
        uint32_t i, q;
        do {
            i = 3;
            value += 2;
            q = floor( sqrt( value ));
            while( i <= q && value % i ) {
                i += 2;
            }
        } while( i <= q );
        return value;
    }
    return value == 2 ? 3 : 2;
}

int testPrime() {
    cout << "-----------------------------------\n";
    cout << "prime numbers for ideal hash length\n";
    cout << "-----------------------------------\n";
    uint32_t value = 0;
    for( uint32_t d = 1; d < ( 2 << 16 ); d = d << 1 ) {
        cout << setfill(' ') << setw( 6 ) << d << ": ";
        for( uint32_t i = 0; value < d ; ++i ) {
            value = nextPrime( value );
        }
        int print = 1.5f + log( d ) / log( 7 );
        for( uint32_t i = 0;; ++i ) {
            auto next = nextPrime( value );
            if( --print > 0 && value < ( d << 1 )) {
                if( i )
                    cout << ", ";
                cout << setfill(' ') << setw( 6 ) << value;
                value = next;
            } else {
                break;
            }
        }
        cout << endl;
    }
    cout << "\n";

    return 0;
}

//-----------------------
int test() {
//-----------------------
    testHash();
    testRng();
    testShuffler();
    testPrime();

    cout.flush();
    return 0;
}

//-----------------------
int main( int argc, char *argv[] ) {
//-----------------------
    return test();
}
//-----------------------
