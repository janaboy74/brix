#ifndef CORESTRING_H_INCLUDED
#define CORESTRING_H_INCLUDED

#include <string>

using namespace std;

struct corestring : public string {
    /* constructor */                   corestring() : string() {};
    /* constructor */                   corestring( const string &src ) : string( src )  {};
    /* constructor */                   corestring( const char *src ) : string( src )  {};
    /* constructor */                   corestring( const char src ) : string( &src, 1 ) {};
    void                                formatva( const char *format, va_list &arg_list );
    void                                format( const char *format, ... );
    int                                 toInt() const;
    long                                toLong() const;
    float                               toFloat() const;
    double                              toDouble() const;
    char                               *get();
    bool                                begins( const char *start ) const;
    void                                operator += ( const char * append );
    /* cast operator */                 operator const char *() const;
};

#endif // BASE_H_INCLUDED
