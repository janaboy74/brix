#include "corestring.h"
#include <cstring>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>

///////////////////////////////////////
void corestring::formatva( const char *format, va_list &arg_list ) {
///////////////////////////////////////
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

///////////////////////////////////////
void corestring::format( const char *format, ... ) {
///////////////////////////////////////
    if( format ) {
        va_list arg_list;
        va_start( arg_list, format );
        formatva( format, arg_list );
        va_end( arg_list );
    }
}

///////////////////////////////////////
int corestring::toInt() const {
///////////////////////////////////////
    return atoi( c_str() );
}

///////////////////////////////////////
long corestring::toLong() const {
///////////////////////////////////////
    return atol( c_str() );
}

///////////////////////////////////////
float corestring::toFloat() const {
///////////////////////////////////////
    return atof( c_str() );
}

///////////////////////////////////////
double corestring::toDouble() const {
///////////////////////////////////////
    return atof( c_str() );
}

///////////////////////////////////////
char *corestring::get() {
///////////////////////////////////////
    return &*begin();
}

///////////////////////////////////////
bool corestring::begins( const char *start ) const {
///////////////////////////////////////
    return !strncmp( c_str(), start, strlen( start ));
}

///////////////////////////////////////
void corestring::operator += ( const char * append ) {
///////////////////////////////////////
    auto length = strlen( append );
    auto prevSize = size();
    resize( prevSize + length );
    strncpy(( char *) &*begin() + prevSize, append, length );
}

///////////////////////////////////////
corestring::operator const char *() const {
///////////////////////////////////////
    return c_str();
}
