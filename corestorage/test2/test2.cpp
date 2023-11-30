#include <iostream>
#include <string>
#include "looprecord.h"
#include "../corestorage.h"
#include <cstring>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

//--------------------------------
void test() {
//--------------------------------
    loopRecord<coreint> record( 10 );

    cout << "--------------------\n";
    cout << "endless record test:\n";
    cout << "--------------------\n";
    for( int j = 9; j <= 22; ++j ) {
        if( j )
            cout << endl;
        record.reset();
        for( int i = 0; i <= j; ++i ) {
            record.add( i );
        }
        cout << "<- ";
        for( int i = 0; i < record.length(); ++i ) {
            if( i )
                cout << ", ";
            cout << record[ i ];
        }
        cout << endl;
        record.reorg();
        cout << "-> ";
        int prev = -1;
        for( int i = 0; i < record.length(); ++i ) {
            if( i )
                cout << ", ";
            auto cur = record[ i ];
            if( cur < prev ) {
                cout << "\nerror\n";
                break;
            } else {
                cout << cur;
            }
            prev = cur;
        }
        cout << endl;
    }
    cout << "----------\n";
    cout << "init test:\n";
    cout << "----------\n";

    coreint cival( 8 );
    printf( "%d\n", cival.get() );
    cival = 424242;
    printf( "%d\n", cival.get() );
    fflush( stdout );
    cout.flush();

    cout << "-------------------\n";
    cout << "vector + init test:\n";
    cout << "-------------------\n";
    corevector<int16_t> testVec( 8, 1 );
    testVec.resize( 3 );
    testVec[ 1 ] = 3;
    for( auto val : testVec ) {
        cout << val << endl;
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
