#include "corestring.h"
#include <iostream>

#define arrayLen( array ) ( sizeof( array ) / sizeof( *array ))
const char *syllables[]={ "no", "ki", "us", "la", "er", "ot", "qa", "ew", "tu", "ba", "uz", "ce" };

//-----------------------
uint32_t get_rnd( uint32_t min, uint32_t max ) {
//-----------------------
    static unsigned int hash = clock();
    hash *= 0x372ce9b9;
    hash += 0xb9e92c37;
    if( min > max )
        return max;
    return min + (( hash << 7 ) ^ ( hash >> 9 ) >> 1 ) % ( max - min );
}

//--------------------------------
void test() {
//--------------------------------
    corestring out;
    for( size_t l = 0; l < 20; ++l ) {
        out.clear();
        const size_t maxWords = get_rnd( 7, 11 );
        for( size_t i = 0; i < maxWords; ++i ) {
            if( i )
                out += " ";
            const size_t maxSyllable = get_rnd( 1, 5 );
            for( size_t j = 0; j < maxSyllable; ++j ) {
                out += syllables[ get_rnd( 0, arrayLen( syllables ))];
            }
        }
        out[ 0 ] = toupper( out[ 0 ]);
        out += ".";
        cout << " out : " << out.get() << endl;
    }
    cout.flush();
}


//--------------------------------
int main(int argc, char *argv[])
//--------------------------------
{
    test();
    return 0;
}
//--------------------------------
