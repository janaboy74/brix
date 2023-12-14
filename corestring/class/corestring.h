#ifndef CORESTRING_H_INCLUDED
#define CORESTRING_H_INCLUDED

#include <string>

namespace core {

struct corestring : public std::string {
    /* constructor */                   corestring() : std::string() {}
    /* constructor */                   corestring( const std::string &src ) : std::string( src )  {}
    /* constructor */                   corestring( const char *src ) : std::string( src )  {}
    /* constructor */                   corestring( const char src ) : std::string( &src, 1 ) {}
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

} // namespace core

#endif // BASE_H_INCLUDED
