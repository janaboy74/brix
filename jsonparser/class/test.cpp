#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "jsonparser.h"

#ifdef __linux
#define O_BINARY 0
#endif

using namespace std;

//--------------------------------
int test( const char *filename ) {
//--------------------------------
    string result;
    int infile = ::open( filename, O_RDONLY | O_BINARY );
    if( infile <= 0 ) {
        return ENOENT;
    }

    struct stat st;
    fstat( infile, &st );
    result.resize( st.st_size );
    size_t readBytes = ::read( infile, &result.front(), result.size() );
    if( STDIN_FILENO != infile )
        close( infile );
    if( st.st_size != readBytes )
        return EIO;

    json json;

    json.parse( result.c_str() );
    cout << "----------------------\n" << filename << "\n----------------------\n";
    cout << json.toString().c_str();
    cout.flush();

    return 0;
}


//--------------------------------
int main(int argc, char *argv[])
//--------------------------------
{
    int result = 0;
    result = test( "../examples/test1.json" );
    if( result )
        return result;
    result = test( "../examples/test2.json" );
    if( result )
        return result;
    return 0;
}
//--------------------------------
