#include <iostream>
#include <cstring>
#include <cstdarg>

#define arrayLen( array ) ( sizeof( array ) / sizeof( *array ))

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
    void reorg() { // This function is not a sorting algorithm! It just reoders the last entries limited to the given number by maintaining it's original order starting with index zero.
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
template <typename T> class loopRecordC : public loopRecord<T> {
//--------------------------------
private:
    size_t number;
public:
    loopRecordC( size_t size = 0 ) : loopRecord<T>( size ), number( 0 ) {};
    void reset() {
        loopRecord<T>::reset();
        number = 0;
    }
    void add( T item ) {
        loopRecord<T>::add( item );
        ++number;
    }
    int count() {
        return number;
    }
};

//-----------------------
struct corestring : public string {
//-----------------------
    corestring() : string() {}
    corestring( const string &src ) : string( src ) {}
    corestring( const char *src ) : string( src ) {}
    corestring( const char src ) : string( &src, 1 ) {}
    void formatva( const char *format, va_list &arg_list ) {
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
    void format( const char *format, ... ) {
        if( format ) {
            va_list arg_list;
            va_start( arg_list, format );
            formatva( format, arg_list );
            va_end( arg_list );
        }
    }
    int toInt() const {
        return atoi( c_str() );
    }
    long toLong() const {
        return atol( c_str() );
    }
    float toFloat() const {
        return atof( c_str() );
    }
    double toDouble() const {
        return atof( c_str() );
    }
    char *get() {
        return &*begin();
    }
    bool begins( const char *start ) const {
        return !strncmp( c_str(), start, strlen( start ));
    }
    void operator += ( const char * append ) {
        auto length = strlen( append );
        auto prevSize = size();
        resize( prevSize + length );
        strncpy(( char *) &*begin() + prevSize, append, length );
    }
    operator const char *() const {
        return c_str();
    };
};

//-----------------------
uint32_t get_rnd( uint32_t min, uint32_t max ) {
//-----------------------
    static unsigned int hash = clock();
    hash *= 0x372ce9b9;
    hash += 0xb9e92c37;
    if( min > max )
        return max;
    return min + (( hash << 7 ) ^ ( hash >> 9 )  >> 1 ) % ( max - min );
}

const char *syllables[]={ "no", "ki", "us", "la", "er", "ot", "qa", "ew", "tu", "ba", "uz", "ce" };

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

    cout << "--- countertest ---\n";
    loopRecordC<unsigned short> crecord( 11 );
    for( unsigned short i = 0 ; i < 7531; ++i ) {
        crecord.add( i );
    }
    crecord.reorg();
    for( int i = 0; i < crecord.length(); ++i ) {
        if( i )
            cout << ", ";
        cout << crecord[ i ];
    }
    cout << "\n\nYou saved: " << crecord.count() - crecord.length() << " records.\n";

    loopRecord<corestring> csrecord( 5 );

    cout << "---------------\n";
    cout << " --- corestring test ---\n";
    for( int e = 3 ; e < 17; ++e ) {
        corestring out;
        const size_t maxWords = get_rnd( 7, 11 );
        for( size_t i = 0; i < maxWords; ++i ) {
            if( i )
                out += " ";
            const size_t maxSyllable = get_rnd( 1, 5 );
            for( size_t j = 0; j < maxSyllable; ++j ) {
                out += syllables[ get_rnd( 0, arrayLen( syllables ))];
            }
        }
        out[ 0 ] = toupper( out[ 0 ]);
        out += ".";
        csrecord.add( out );
        cout << out << endl;
    }
    csrecord.reorg();
    cout << " --- reorded ---\n";
    for( int i = 0; i < csrecord.length(); ++i ) {
        cout << csrecord[ i ] << endl;
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
