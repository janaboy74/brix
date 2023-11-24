#include <iostream>
#include <cstring>
#include <cstdarg>
#include <cinttypes>
#include <fcntl.h>
#include <unistd.h>

#ifdef __linux
#define O_BINARY 0
#endif

#define arrayLen( array ) ( sizeof( array ) / sizeof( *array ))

using namespace std;

//-----------------------
struct corestring : public std::string {
//-----------------------
    corestring() : std::string() {}
    corestring( const std::string &src ) : std::string( src ) {}
    corestring( const char *src ) : std::string( src ) {}
    corestring( const char src ) : string( &src, 1 ) {}
    void formatva( const char *format, va_list &arg_list ) {
        if( format ) {
            va_list cova = {{}};
            va_copy( cova, arg_list );
            int size = vsnprintf( nullptr, 0, format, cova );
            va_end( arg_list );
            resize( size );
            va_copy( cova, arg_list );
            vsnprintf( &at( 0 ), size + 1, format, cova );
            va_end( arg_list );
        }
    }
    void format( const char *format, ... ) {
        if( format ) {
            va_list arg_list;
            va_start( arg_list, format );
            formatva( format, arg_list );
            va_end( arg_list );
        }
    }
    int toInt() const {
        return atoi( c_str() );
    }
    long toLong() const {
        return atol( c_str() );
    }
    float toFloat() const {
        return atof( c_str() );
    }
    double toDouble() const {
        return atof( c_str() );
    }
    char *get() {
        return &*begin();
    }
    bool begins( const char *start ) const {
        return !strncmp( c_str(), start, strlen( start ));
    }
    void operator += ( const char * append ) {
        auto length = strlen( append );
        auto prevSize = size();
        resize( prevSize + length );
        strncpy(( char *) &*begin() + prevSize, append, length );
    }
    operator const char *() const {
        return c_str();
    };
};

//-----------------------
class rng {
//-----------------------
    uint32_t hash;
    void flip() {
        hash *= 0x372ce9b9;
        hash += 0xb9e92c37;
    }
public:
    rng( uint32_t initval = 0x372ce9b9 ) {
        init( initval  );
    }
    ~rng() {
    }
    void init( uint32_t initval = 0x372ce9b9 ) {
        hash = initval;
        flip();
    }
    void initRnd() {
        init( clock() );
    }
    uint32_t getRnd() {
        flip();
        return (( hash << 7 ) ^ ( hash >> 9 ))  >> 1;
    }
    uint32_t getRnd( uint32_t min, uint32_t max ) {
      if( min > max )
          return max;
      return ( getRnd() % ( max - min )) + min;
    }
};

//-----------------------
uint32_t get_rnd( uint32_t min, uint32_t max ) {
//-----------------------
    static unsigned int hash = clock();
    hash *= 0x372ce9b9;
    hash += 0xb9e92c37;
    if( min > max )
        return max;
    return min + (( hash << 7 ) ^ ( hash >> 9 )  >> 1 ) % ( max - min );
}

const char *syllables[]={ "no", "ki", "us", "la", "er", "ot", "qa", "ew", "tu", "ba", "uz", "ce" };

//-----------------------
void test() {
//-----------------------
    rng rnd;
    rnd.initRnd();
    uint32_t sum( 0 );

    const size_t count = 1000;
    for( size_t i = 0; i < count; ++i ) {
        sum += rnd.getRnd( 10, 21 );
    }
    cout << " avg : " << 1.f * sum / count << endl;

    corestring out;
    const size_t maxWords = get_rnd( 7, 11 );
    for( size_t i = 0; i < maxWords; ++i ) {
        if( i )
            out += " ";
        const size_t maxSyllable = get_rnd( 1, 5 );
        for( size_t j = 0; j < maxSyllable; ++j ) {
            out += syllables[ rnd.getRnd( 0, arrayLen( syllables ))];
        }
    }
    out[ 0 ] = toupper( out[ 0 ]);
    out += ".";
    cout << " out : " << out << endl;
}

//-----------------------
int main( int argc, char *argv[] ) {
//-----------------------
    test();
    return 0;
}
//-----------------------
