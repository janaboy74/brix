#include <iostream>

using namespace std;

//--------------------------------
template <typename T> class loopRecord {
//--------------------------------
private:
    size_t size;
    size_t filled;
    size_t pos;
    T *array;
public:
    loopRecord( size_t size = 0 ) : filled( 0 ), size( size ), pos( 0 ), array( nullptr ) {
        if( size )
            resize( size );
    };
    ~loopRecord() {
        delete[] array;
    }
    void resize( size_t size ) {
        if( array )
            delete[] array;
        array = new T[ size ];
        pos = 0;
    }
    void reset() {
        pos = 0;
        filled = 0;
    }
    void add( T item ) {
        if( pos >= size )
            pos = 0;
        array[ pos ] = item;
        ++pos;
        if( filled != size ) {
            ++filled;
            if( filled > size )
                filled = size;
        }
    }
    T &operator []( size_t itemID ) {
        return get( itemID );
    }
    T &get( size_t itemID ) {
        if( itemID < size )
            return array[ itemID ];
        static T failure;
        return failure;
    }
    int length() {
        return filled;
    }
    void reorg() {
        int cur = 0, delta = filled - pos;
        if( pos >= filled )
            return;
        for( ;; ) {
            swap( array[ cur++ ], array[ pos++ ]);
            if( pos >= filled ) {
                if( delta >= pos - cur )
                    delta %= pos - cur;
                if( 0 == delta )
                    break;
                pos -= delta;
            }
        }
    }
};

//--------------------------------
void test() {
//--------------------------------
    loopRecord<int> record( 10 );
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
