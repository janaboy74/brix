#include <iostream>
#include <string>
#include "looprecord.h"
#include "../corestorage"
#include <cstring>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <cmath>

using namespace core;

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

    coremodmap<char, int> assign;
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
    coremodmap< float, float > sintable, costable;
    for( int i = 0; i <= 16; ++i ) {
        float angle = i * M_PI / 4.f;
        sintable.insert( angle, sin( angle ));
        costable.insert( angle, cos( angle + M_PI / 8 ));
    }
    for( auto &item : sintable ) {
        if( item.second < 1e-5f )
            mathmod.remove( item.first );
        else if( item.second == 1 )
            crossonekiller.remove( item.first );
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
    cout << "<set>: ";
    coremodset< char > settable;
    for( auto &chr : "This is a test string" ) {
        if( chr )
            settable.insert( chr );
    }
    for( auto &chr : settable ) {
        if( &chr != &*settable.begin() )
            cout << ", ";
        cout << "'" << chr << "'";
    }
    cout << endl;
    for( auto &chr : settable ) {
        if( isupper( chr ))
            settable.modificator().remove( chr );
    }
    settable.update();
    cout << "after: ";
    for( auto &chr : settable ) {
        if( &chr != &*settable.begin() )
            cout << ", ";
        cout << "'" << chr << "'";
    }
    cout << endl;
    corelist<char> listinitlisttest { 'a', 'b', 'c' };
    cout << "corelist initializer list test : ";
    for( auto &chr : listinitlisttest ) {
        if( &chr != &*listinitlisttest.begin() )
            cout << ", ";
        cout << "'" << chr << "'";
    }
    cout << endl;
    corevector<char> vectorinitlisttest { 'a', 'b', 'c' };
    cout << "corevector initializer list test : ";
    for( auto &chr : vectorinitlisttest ) {
        if( &chr != &*vectorinitlisttest.begin() )
            cout << ", ";
        cout << "'" << chr << "'";
    }
    cout << endl;
    pairvector<char, int> pairvectorinitlisttest {{ 'b', 2 }, { 'c', 3 }, { 'a', 1 }};
    cout << "pairvector initializer list test : ";
    for( auto &item : pairvectorinitlisttest ) {
        if( &item != &*pairvectorinitlisttest.begin() )
            cout << ", ";
        cout << "{ " << item.first << " = " << item.second << " }";
    }
    cout << endl;
    coremodset<char> setinitlisttest { 'a', 'b', 'c' };
    cout << "coremodset initializer list test : ";
    for( auto &chr : setinitlisttest ) {
        if( &chr != &*setinitlisttest.begin() )
            cout << ", ";
        cout << "'" << chr << "'";
    }
    cout << endl;
    coremodmap<char, int> mapinitlisttest {{ 'b', 2 }, { 'c', 3 }, { 'a', 1 }};
    cout << "coremodmap initializer list test : ";
    for( auto &item : mapinitlisttest ) {
        if( &item != &*mapinitlisttest.begin() )
            cout << ", ";
        cout << "{ " << item.first << " = " << item.second << " }";
    }
    cout << endl;
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
