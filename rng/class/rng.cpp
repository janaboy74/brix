#include "rng.h"
#include <iostream>

using namespace std;

//-----------------------
void rng::flip() {
//-----------------------
    hash *= 0x372ce9b9;
    hash += 0xb9e92c37;
}

//-----------------------
rng::rng( int32_t initval ) {
//-----------------------
    init( initval  );
}

//-----------------------
rng::~rng() {
//-----------------------
}

//-----------------------
void rng::init( int32_t initval ) {
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
int32_t rng::getRnd() {
//-----------------------
    flip();
    return (( hash << 7 ) ^ ( hash >> 9 ))  >> 1;
}

//-----------------------
int32_t rng::getRnd( int32_t min, int32_t max ) {
//-----------------------
  if( min > max )
      return max;
  return ( getRnd() % ( max - min )) + min;
}

//-----------------------
int32_t get_rnd( int32_t min, int32_t max ) {
//-----------------------
    static unsigned int hash = ( min == max ) ? clock() : min;
    hash *= 0x372ce9b9;
    hash += 0xb9e92c37;
    if( min >= max )
        return max;
    return min + (( hash << 7 ) ^ ( hash >> 9 )  >> 1 ) % ( max - min );
}
