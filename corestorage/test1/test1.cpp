#include <iostream>
#include <string>
#include "../corestorage"
#include <cstring>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
using namespace core;

//--------------------------------
struct corestring : public string {
//--------------------------------
    corestring() : string() {}
    corestring( const string &src ) : string( src ) {}
    corestring( const char *src ) : string( src ) {}
    corestring( const char src ) : string( &src, 1 ) {}
    void formatva( const char *format, va_list &arg_list ) {
        if( format ) {
            va_list cova;
            va_copy( cova, arg_list );
            int size = vsnprintf( NULL, 0, format, cova );
            va_end( arg_list );
            string::resize( size );
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
    operator const char *() {
        return c_str();
    }
};

//--------------------------------
void test() {
//--------------------------------
    map<corestring, coreint> testValues={{ "vector", 1 }, { "list", 1 }, { "map", 2 }, { "set", 1 }};

    corevector<corestring> vectorstrtest;
    corevector<coreint> vectorinttest;
    corelist<corestring> liststrtest;
    corelist<coreint> listinttest;
    coremap<corestring, coreint> maptest;
    coreset<corestring> setstrtest;
    coreset<coreint> setinttest;
    corevector<coresize_t> vectorsize_ttest;
    for( auto val : testValues ) {
        vectorstrtest.push_back( val.first );
        vectorinttest.push_back( val.second );
        liststrtest.push_back( val.first );
        listinttest.push_back( val.second );
        maptest.insert( val.first, val.second );
        setstrtest.insert( val.first );
        setinttest.insert( val.second );
        vectorsize_ttest.push_back( ( size_t ) val.second );
    }
    cout << "\rvectorstrtest:\n";
    for( auto &val : vectorstrtest ) {
        cout << val << " ";
    }
    cout << "\nvectorinttest:\n";
    for( auto &val : vectorinttest ) {
        cout << val << " ";
    }
    cout << "\nliststrtest:\n";
    for( auto &val : liststrtest ) {
        cout << val << " ";
    }
    cout << "\nlistinttest:\n";
    for( auto &val : listinttest ) {
        cout << val << " ";
    }
    cout << "\nmaptest:\n";
    for( auto &val : maptest ) {
        cout << " [ " << val.first << ", " << val.second << " ] ";
    }
    cout << "\nsetstrtest:\n";
    for( auto &val : setstrtest ) {
        cout << val << " ";
    }
    cout << "\nsetinttest:\n";
    for( auto val : setinttest ) {
        cout << val << " ";
    }
    cout << "\nvectorsize_ttest:\n";
    for( auto &val : vectorsize_ttest ) {
        cout << val << " ";
    }
    cout << endl;
    cout << "maptest[ \"invalid\" ]: " << maptest[ "invalid" ] << endl;
    cout << "setstrtest.contains( \"invalid\" ) : " << ( setstrtest.contains( "invalid" ) ? "contains" : "not contains") << endl;
    cout << "setinttest.contains( 44 ) : " << ( setinttest.contains( 44 ) ? "contains" : "not contains") << endl;
    cout << "setinttest.contains( 1 ) : " << ( setinttest.contains( 1 ) ? "contains" : "not contains") << endl;
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
