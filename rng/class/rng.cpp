#include "rng.h"
#include <iostream>
#include <cstring>

using namespace std;

//-----------------------
void rng::flip() {
//-----------------------
    hash *= 0x372ce9b9;
    hash += 0xb9e92c37;
}

//-----------------------
rng::rng( rngType initval ) {
//-----------------------
    init( initval  );
}

//-----------------------
rng::~rng() {
//-----------------------
}

//-----------------------
void rng::init( rngType initval ) {
//-----------------------
    hash = initval;
    flip();
}

//-----------------------
void rng::initRnd() {
//-----------------------
    init( clock() );
}

//-----------------------
rng::rngType rng::getRnd() {
//-----------------------
    flip();
    return (( hash << 7 ) + ( hash >> 9 ));
}

//-----------------------
rng::rngType rng::getRnd( rngType min, rngType max ) {
//-----------------------
    if( min > max )
        return max;
    return (( getRnd() >> 1 ) % ( max - min )) + min;
}

//-----------------------
int32_t get_rnd( int32_t min, int32_t max ) {
//-----------------------
    static uint32_t hash = ( min == max ) ? clock() : min;
    hash *= 0x372ce9b9;
    hash += 0xb9e92c37;
    if( min >= max )
        return max;
    return min + (( hash << 7 ) ^ ( hash >> 9 )  >> 1 ) % ( max - min );
}

//-----------------------
hashing::hashing( const char *hash_init, size_t hashLength ) {
//-----------------------
    resize( hash_init, hashLength );
};

//-----------------------
void hashing::init( const char *hash_init, int count ) {
//-----------------------
    rng::init( 0x98a410f6 );
    memset( currentHash.get(), 0xa5, hashLength * sizeof( hashType ));
    if( nullptr == hash_init ) {
        return;
    }
    addHash( hash_init, strlen( hash_init ), count );
}

//-----------------------
void hashing::resize( const char *hash_init, size_t hashLength ) {
//-----------------------
    this->hashLength = hashLength;
    if( this->hashLength < sizeof( rngType ))
        this->hashLength = sizeof( rngType );
    currentHash = shared_ptr<hashType[]>( new hashType[ this->hashLength]);
    init( hash_init );
}

//-----------------------
void hashing::genHash( const char *src, int count ) {
//-----------------------
    genHash( src, strlen( src ), count );
}

//-----------------------
void hashing::genHash( const void *src, size_t srcLength, int count ) {
//-----------------------
    rng::init( 0x98a410f6 );
    memset( currentHash.get(), 0xa5, hashLength * sizeof( hashType ));
    if( srcLength > 1 )
        addHash(( const char *)src + 1, srcLength - 1, count );
    addHash( src, srcLength, count );
}

//-----------------------
void hashing::addHash( const char *src, int count ) {
//-----------------------
    addHash( src, strlen( src ), count );
}

//-----------------------
void hashing::addHash( const void *src, size_t srcLength, int count ) {
//-----------------------
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

//-----------------------
template<class T> T hashing::singlehex( T ch, bool uppercase ){
//-----------------------
    return ch < 10 ? ch + '0' : ch - 10 + ( uppercase ? 'A' : 'a' );
}

//-----------------------
uint16_t hashing::toHex( uint8_t ch, bool uppercase ) {
//-----------------------
    return hashing::singlehex<uint8_t>( ch >> 4, uppercase ) << 8 | hashing::singlehex<uint8_t>( ch & 0xf, uppercase );
}

//-----------------------
string hashing::toHexString( bool uppercase, bool prefix ) {
//-----------------------
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

//-----------------------
void hashing::printHash( bool prefix, bool uppercase ) {
//-----------------------
    cout << toHexString( prefix, uppercase ) << endl;
}
