#include <looprecord.h>

//--------------------------------
int divide( int number, int divider ) {
//--------------------------------
    if( divider == 0 )
        throw "division by zero exception";
    return number / divider;
}

//--------------------------------
void test() {
//--------------------------------
    loopRecord<int> record( 10 );

    cout << "--------------------\n";
    cout << "endless record test:\n";
    cout << "--------------------\n";
    for( int j = 0; j <= 512; ++j ) {
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

    cout << "\n---------------\n";
    cout << "exception text:\n";
    cout << "---------------\n";
    record.reset();
    for( int i = 0 ; i < 42; ++i ) {
        record.add( i );
        try {
            int z = divide( 42, i - 21 );
        } catch ( const char *error ) {
            cout << "problem( " << error << " ) with loop at " << i << endl;
            record.reorg();
            for( int i = 0; i < record.length(); ++i ) {
                if( i )
                    cout << ", ";
                cout << record[ i ];
            }
            cout << endl;
        }
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
