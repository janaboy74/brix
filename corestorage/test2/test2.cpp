#include <iostream>
#include <string>
#include "looprecord.h"
#include "../corestorage.h"
#include <cstring>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cmath>

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

    coremap<char, int> assign;
    assign.insert( 'C', 1 );
    assign.insert( 'a', 5 );
    assign.insert( 'z', 8 );
    assign.insert( 'q', 9 );
    assign.insert( 's', 88 );
    assign.insert( 'Q', 12 );
    cout << "\n--- map modificator test ---\n";
    cout << "map orig:\n";
    for( auto item : assign ) {
        if( item.first == 'q' )
            assign.modificator().remove( 'z' );
        else if( item.first == 'a' )
            assign.modificator().insert( 'b', 6 );
        else if( item.first == 's' )
            assign.modificator().remove( 'N' );
        cout << item.first << '=' << item.second << endl;
    }
    assign.modificator().insert( 'U', 66 );
    cout << "map untouched:\n";
    for( auto item : assign ) {
        cout << item.first << '=' << item.second << endl;
    }
    assign.update();
    cout << "map final:\n";
    for( auto item : assign ) {
        cout << item.first << '=' << item.second << endl;
    }

    cout << "\n--- external map modificator test ---\n";
    coremapmodificator< float, float > mathmod;
    coremapmodificator< float, float > crossonekiller;
    coremap< float, float > sintable, costable;
    for( int i = 0; i <= 16; ++i ) {
        float angle = i * M_PI / 4.f;
        sintable.insert( angle, sin( angle ));
        costable.insert( angle, cos( angle + M_PI / 8 ));
    }
    for( auto it = sintable.begin() ; it != sintable.end(); ++it ) {
        if( it->second < 1e-5f )
            mathmod.remove( it->first );
        else if( it->second == 1 )
            crossonekiller.remove( it->first );
    }
    sintable.update( &mathmod );
    costable.update( &mathmod );
    costable.update( &crossonekiller );
    for( auto &tableItem : sintable ) {
        cout << "sin: " << tableItem.first << '=' << tableItem.second << endl;
    }
    cout << "<<>>\n";
    for( auto &tableItem : costable ) {
        cout << "cos: " << tableItem.first << '=' << tableItem.second << endl;
    }
    mathmod.cleanup();
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
