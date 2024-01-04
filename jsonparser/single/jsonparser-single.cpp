#include <iostream>
#include <cstring>
#include <cstdarg>
#include <cinttypes>
#include <memory>
#include <set>
#include <vector>
#include <map>
#include <iterator>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifdef __linux
#define O_BINARY 0
#endif

//-----------------------
struct corestring : public std::string {
//-----------------------
    corestring() : std::string() {}
    corestring( const std::string &src ) : std::string( src ) {}
    corestring( const char *src ) : std::string( src ) {}
    corestring( const char src ) : std::string( &src, 1 ) {}
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
    void operator += ( const corestring &append ) {
        operator +=( append.c_str() );
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

class corevectormodificator {
    std::set<size_t> removed;
    std::set<size_t> inserted;
    template<class X> friend class corevector;
public:
    void insert( const size_t index ) {
        if( removed.find( index ) != removed.end() )
            removed.erase( index );
        inserted.insert( index );
    }
    void remove( const size_t index ) {
        if( removed.find( index ) != removed.end() )
            removed.erase( index );
        inserted.insert( index );
    }
    void cleanup() {
        removed.clear();
        inserted.clear();
    }
};

//--------------------------------
template <class V> struct corevector : public std::vector<V> {
//--------------------------------
    std::shared_ptr<corevectormodificator> vectormodificator;
public:
    corevector() : std::vector<V>() {}
    corevector( const size_t s, const V v ) {
        std::vector<V>::resize( s );
        for( auto &var : *this )
            var = v;
    }
    V &operator[]( const size_t index ) {
        static V dummy;
        if( index < this->size() )
            return std::vector<V>::operator[]( index );
        return dummy;
    }
    corevectormodificator &modificator() {
        if( !vectormodificator.get() )
            vectormodificator = std::make_shared<corevectormodificator>();
        return *vectormodificator.get();
    }
    void update() {
        if( !vectormodificator.get() )
            return;
        for( auto &item : vectormodificator->removed ) {
            std::vector<V>::remove( item );
        }
        for( auto &item : vectormodificator->inserted ) {
            std::vector<V>::insert( item );
        }
    }
};

template <class I, class V> class coremapmodificator {
    std::set< I> removed;
    std::map< I, V> inserted;
    template<class X, class Y> friend class coremap;
public:
    void insert( const std::pair< I, V > item ) {
        insert( item.first, item.second );
    }
    void insert( const I item, const V val ) {
        if( removed.find( item ) != removed.end() )
            removed.erase( item );
        inserted.insert( std::pair<I, V>( item, val ));
    }
    void remove( const I item ) {
        if( inserted.find( item ) != inserted.end() )
            inserted.erase( item );
        removed.insert( item );
    }
    void cleanup() {
        removed.clear();
        inserted.clear();
    }
};

//--------------------------------
template <class I, class V> class coremap : public std::map<I, V> {
//--------------------------------
    std::shared_ptr<coremapmodificator<I, V>> mapmodificator;
public:
    V &operator[]( const I item ) {
        static V dummy;
        if( contains( item ))
            return std::map<I, V>::operator[]( item );
        return dummy;
    }
    void insert( const I item, const V val ) {
        std::map<I, V>::insert( std::pair<I, V>( item, val ));
    }
    bool contains( const I item ) const {
        return this->find( item ) != this->end();
    }
    coremapmodificator<I, V> &modificator() {
        if( !mapmodificator.get() )
            mapmodificator = std::make_shared<coremapmodificator<I, V>>();
        return *mapmodificator.get();
    }
    void update( coremapmodificator<I, V> *modificator = nullptr ) {
        auto &executor = modificator ? ( *modificator) : ( *mapmodificator.get() );
        if( !&executor )
            return;
        for( auto &item : executor.removed ) {
            std::map<I,V>::erase( item );
        }
        for( auto &item : executor.inserted ) {
            std::map<I,V>::insert( std::pair<I, V>( item.first, item.second ));
        }
        if( !modificator )
            mapmodificator->cleanup();
    }
};

//--------------------------------
template <class I, class V, class Alloc = std::allocator<std::pair<I, V>>> struct pairvector : public std::vector<std::pair<I, V>, Alloc> {
    //--------------------------------
    pairvector() : std::vector<std::pair<I, V>, Alloc>() {}
    pairvector( std::initializer_list<std::pair<const I, const V>> list, const Alloc& alloc = Alloc() ) : std::vector<std::pair<I, V>, Alloc>( list, alloc ) {}
    void push_back( I item, V val ) {
        std::vector<std::pair<I,V>>::push_back( std::pair<I, V>( item, val ));
    }
    std::pair<I,V> &operator[]( const size_t index ) {
        static std::pair<I,V> dummy;
        if( index < this->size() )
            return std::vector<std::pair<I,V>>::operator[]( index );
        return dummy;
    }
};

//--------------------------------
struct jsonItem {
//--------------------------------
    enum itemType {
        IT_DEFAULT          = 0,
        IT_NODE_ARRAY       = 0x10,
        IT_NODE_ARRAY_SUB   = 0x01 | IT_NODE_ARRAY,
        IT_NODE_ARRAY_LIST  = 0x02 | IT_NODE_ARRAY
    };

    int8_t itemType = IT_DEFAULT;
    coremap<std::string, std::string> values;
    corevector<std::shared_ptr<jsonItem>> subItems;
    corevector<std::string> array;
    coremap<std::string, std::shared_ptr<jsonItem>> nodes;

    jsonItem() {}
    jsonItem( const std::shared_ptr<jsonItem> other ) {
        operator = ( other );
    };
    jsonItem( const jsonItem &other ) {
        operator = ( other );
    };
    ~jsonItem() {};
    void operator = ( const jsonItem &other ) {
        values = other.values;
        subItems = other.subItems;
        array = other.array;
        nodes = other.nodes;
    }
    std::shared_ptr<jsonItem> operator = ( std::shared_ptr<jsonItem> other ) {
        values = other->values;
        subItems = other->subItems;
        array = other->array;
        nodes = other->nodes;
        return other;
    }
    bool empty() {
        return !( nodes.size() || subItems.size() || array.size() || values.size() );
    }
};

// simple but powerful json parser
//--------------------------------
class json {
//--------------------------------
    enum buildTypes {
        BT_DEFAULT              = 0,
        BT_USE_IDENT            = 0x0100,
        BT_INCREASE_IDENT       = 0x0200,
        BT_DECREASE_IDENT       = 0x0400,
        BT_NEWLINE              = 0x0800,
        BT_TYPE_MASK            = 0x00ff,
        BT_ITEM                 = 0x0001,
        BT_FINAL                = 0x00fe,
    };
public:
        std::shared_ptr<jsonItem> main;
        json() {}
        int parse( const char *jsonText ) { // return the error code - see below
            std::vector<std::shared_ptr<jsonItem>> node;
            std::vector<std::string> text;
            std::vector<char> stack;
            std::string str;

            main = std::make_shared<jsonItem>();
            if( !jsonText || !strlen( jsonText ))
                return 1; // empty json file;

            bool quote = false;
            bool escape = false;

            // splitting into nodes
            //--------------------------------
            for( auto ch = jsonText; *ch; ++ch ) {
                if( quote ) {
                    if( escape ) {
                        if( '\"' != *ch )
                            str += '\\';
                        str += *ch;
                        escape = false;
                        continue;
                    } else if( '\\' == *ch ) {
                        escape = true;
                        continue;
                    }
                }
                if( '"' == *ch ) {
                    if( quote ) {
                        text.push_back( str );
                        str.clear();
                    }
                    quote = !quote;
                    continue;
                }
                if( quote ) {
                    str += *ch;
                    continue;
                }
                switch( *ch ) {
                    case '\n': case ' ': case '\t':
                        break;
                    case '[': case ']': case '{': case '}': case ',': case ':':
                        if( str.length() )
                            text.push_back( str );
                        str.clear();
                        text.push_back( corestring( *ch ));
                        break;
                    default:
                        str += *ch;
                        break;
                }
            }

            // parsing the nodes and writing it into json structure
            //--------------------------------
            std::shared_ptr<jsonItem> curItem;
            if( !text.size() || text.front() != "[" ) {
                main = std::make_shared<jsonItem>();
                node.push_back( main );
                curItem = node.back();
            }
            std::string name, val, lastNode = "";
            char lastStack = 0;
            while( text.size() ) {
                if( stack.size() )
                    lastStack = stack.back();
                if( text.size() )
                    lastNode = text.front();
                if( "[" == lastNode ) {
                    stack.push_back( *text.front().begin() );
                    std::shared_ptr<jsonItem> newItem = std::make_shared<jsonItem>();
                    newItem->itemType = jsonItem::IT_NODE_ARRAY_SUB;
                    stack.push_back( '.' );
                    if( name.size() ) {
                        curItem->nodes.insert( name, newItem );
                        name.clear();
                    } else if( curItem ) {
                        curItem->subItems.push_back( newItem );
                    } else {
                        main = newItem;
                    }
                    node.push_back( newItem );
                    curItem = node.back();
                    text.erase( text.begin() );
                } else if( "]" == lastNode ) {
                    stack.pop_back();
                    node.erase( node.end() );
                    text.erase( text.begin() );
                    if( node.empty() && text.size() ) {
                        return 2; // too much closing brackets
                    }
                    if( node.size() )
                        curItem = node.back();
                    else
                        break;
                } else if( "{" == lastNode ) {
                    if( '.' == lastStack ) {
                        std::shared_ptr<jsonItem> newItem = std::make_shared<jsonItem>();
                        curItem->subItems.push_back( newItem );
                        node.push_back( newItem );
                        curItem = node.back();
                        stack.push_back( *lastNode.begin() );
                        text.erase( text.begin() );
                        name.clear();
                        continue;
                    }
                    std::shared_ptr<jsonItem> newItem = std::make_shared<jsonItem>();
                    if( name.size() )
                        curItem->nodes.insert( name, newItem );
                    else
                        curItem->subItems.push_back( newItem );
                    stack.push_back( *lastNode.begin() );
                    node.push_back( newItem );
                    curItem = node.back();
                    text.erase( text.begin() );
                } else if( "," == lastNode ) {
                    text.erase( text.begin() );
                } else if( "}" == lastNode ) {
                    stack.pop_back();
                    node.erase( node.end() );
                    text.erase( text.begin() );
                    if( node.empty() && text.size() ) {
                        return 2; // too much closing brackets
                    }
                    curItem = node.back();
                } else if( text.size() > 1 ) {
                    if( text[1] == ":" ) {
                        name = text.front();
                        text.erase( text.begin() );
                        text.erase( text.begin() );
                        if( text.size() ) {
                            if( "[" == text.front() ||
                                "]" == text.front() ||
                                "{" == text.front() ||
                                "}" == text.front() )
                                continue;
                            val = text.front();
                            text.erase( text.begin() );
                            curItem->values.insert( name, val );
                        }
                        name.clear();
                    } else {
                        curItem->array.push_back( lastNode );
                        text.erase( text.begin() );
                    }
                } else if( '.' == lastStack ) {
                    curItem->itemType = jsonItem::IT_NODE_ARRAY_LIST;
                    if( name.size() ) {
                        name.clear();
                    } else if( "," == lastNode ) {
                        text.erase( text.begin() );
                    } else {
                        curItem->nodes.insert( lastNode, nullptr );
                        text.erase( text.begin() );
                    }
                    continue;
                } else {
                    return 3; // parser error / invalid json source
                    break;
                }
            }
            if( !( main->itemType &= jsonItem::IT_NODE_ARRAY ) && main->subItems.size() )
                main = *main->subItems.begin();
            return 0; // success
        }
        pairvector< uint16_t, corestring > nodeToString( pairvector< uint16_t, corestring > &output, std::shared_ptr<jsonItem> item ) {
            if( item->itemType &= jsonItem::IT_NODE_ARRAY ) {
                output.push_back( BT_INCREASE_IDENT | BT_USE_IDENT | BT_ITEM, "[" );
            } else {
                output.push_back( BT_INCREASE_IDENT | BT_USE_IDENT | BT_ITEM, "{" );
            }
            output.push_back( BT_NEWLINE, "" );
            if( item->array.size() ) {
                for( auto &node : item->array ) {
                    if( &node != &*item->array.begin() )
                        output.push_back( BT_NEWLINE | BT_ITEM, ", " );
                    auto &realValue = node;
                    const char *src = "\"";
                    const char *dest = "\\\"";
                    size_t pos = 0;
                    for( ;; ) {
                        pos = realValue.find( src, pos );
                        if( pos == std::string::npos )
                            break;
                        realValue.replace( pos, strlen( src ), "\\\"" );
                        pos += strlen( dest );
                    }
                    if(( "false" == realValue || "true" == realValue || "null" == realValue ) && realValue.length() ) {
                        output.push_back( BT_USE_IDENT | BT_ITEM, realValue );
                    } else {
                        output.push_back( BT_USE_IDENT | BT_ITEM, '"' );
                        output.push_back( BT_ITEM, realValue );
                        output.push_back( BT_ITEM, '"' );
                    }
                }
                item->array.clear();
                if( !item->empty() )
                    output.push_back( BT_ITEM, "," );
            }
            if( item->values.size() ) {
                for( auto &value : item->values ) {
                    if( &value != &*item->values.begin() )
                        output.push_back( BT_NEWLINE | BT_ITEM, ", " );
                    output.push_back( BT_USE_IDENT | BT_ITEM, '"' );
                    output.push_back( BT_ITEM, value.first );
                    output.push_back( BT_ITEM, '"' );
                    output.push_back( BT_ITEM, " : " );
                    auto &realValue = value.second;
                    const char *src = "\"";
                    const char *dest = "\\\"";
                    size_t pos = 0;
                    for( ;; ) {
                        pos = realValue.find( src, pos );
                        if( pos == std::string::npos )
                            break;
                        realValue.replace( pos, strlen( src ), "\\\"" );
                        pos += strlen( dest );
                    }
                    if(( "false" == realValue || "true" == realValue || "null" == realValue ) && realValue.length() ) {
                        output.push_back( BT_ITEM, realValue );
                    } else {
                        output.push_back( BT_ITEM, '"' );
                        output.push_back( BT_ITEM, realValue );
                        output.push_back( BT_ITEM, '"' );
                    }
                }
                item->values.clear();
                if( !item->empty() ) {
                    output.push_back( BT_NEWLINE | BT_ITEM, "," );
                }
            }
            if( item->nodes.size() ) {
                for( auto &node : item->nodes ) {
                    if( &node != &*item->nodes.begin() )
                        output.push_back( BT_NEWLINE | BT_ITEM, ", " );
                    output.push_back( BT_USE_IDENT | BT_ITEM, '"' );
                    output.push_back( BT_ITEM, node.first );
                    output.push_back( BT_ITEM, '"' );
                    output.push_back( BT_ITEM, " : " );
                    output.push_back( BT_NEWLINE, "" );
                    nodeToString( output, node.second );
                }
                item->nodes.clear();
                if( !item->empty() ) {
                    output.push_back( BT_NEWLINE | BT_ITEM, "," );
                }
            }
            if( item->subItems.size() ) {
                for( auto &node : item->subItems ) {
                    if( &node != &*item->subItems.begin() )
                        output.push_back( BT_NEWLINE | BT_ITEM, ", " );
                    nodeToString( output, node );
                }
                item->subItems.clear();
            }
            output.push_back( BT_NEWLINE, "" );
            if( item->itemType &= jsonItem::IT_NODE_ARRAY ) {
                output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM, "]" );
            } else {
                output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM, "}" );
            }
            return output;
        }
        std::string toString( int identLength = 2 ) {
            std::string itentString;
            itentString.resize( identLength );
            memset( &*itentString.begin(), ' ', itentString.length() );

            pairvector< uint16_t, corestring > output;
            nodeToString( output, main );
            output.push_back( BT_NEWLINE, "" );

            std::string ident;
            std::string outputstring;
            char *out = 0;
            bool predict = true;
            size_t size = 0;

            for( auto item = output.begin();; ++item ) {
                if( item == output.end() ) {
                    if( predict ) {
                        ident.clear();
                        predict = false;
                        item = output.begin();
                        outputstring.resize( size );
                        out = &outputstring.at( 0 );
                        size = 0;
                    } else {
                        break;
                    }
                }
                if( item->first & BT_DECREASE_IDENT ) {
                    if( ident.size() >= identLength )
                        ident.resize( ident.size() - identLength );
                }
                if( item->first & BT_USE_IDENT ) {
                    if( predict ) {
                        size += strlen( ident.c_str() );
                    } else {
                        out = addString( out, ident.c_str() );
                    }
                }
                if( item->first & BT_ITEM ) {
                    if( predict ) {
                        size += strlen( item->second.c_str() );
                    } else {
                        out = addString( out, item->second.c_str() );
                    }
                }
                if( item->first & BT_NEWLINE ) {
                    if( predict ) {
                        ++size;
                    } else {
                        out = addString( out, "\n" );
                    }
                }
                if( item->first & BT_INCREASE_IDENT ) {
                    ident += itentString;
                }
            }
            return outputstring;
        }
        char *addString( char *dest, const char *add ) {
            while( *add )
                *(dest++) = *( add++ );
            *dest = 0;
            return dest;
        }
        bool empty() {
            return main->empty();
        }
};

//-----------------------
int test( int argc, char *argv[] ) {
//-----------------------
    std::string result;
    int infile = STDIN_FILENO;

    if( argc > 1 ) {
        infile = ::open( argv[ 1 ], O_RDONLY | O_BINARY );
        if( infile <= 0 ) {
            infile = STDIN_FILENO;
            return ENOENT;
        }
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

    switch( auto retval = json.parse( result.c_str() )) { // can be replaced with own error handling
    case 2:
        std::cerr << "Too much closing brackets in json source.\n";
        break;
        break;
    case 3:
        std::cerr << "Parser error or invalid json source.\n";
        break;
    default:
        if( retval <=1 ) {
            std::cerr << "The json source looks ok.\n";
            std::cout << json.toString().c_str();
        } else
            std::cerr << "Unknown error in json source.\n";
        break;
    }

    std::cout.flush();
    fflush( stdout );

    return 0;
}

//-----------------------
int main( int argc, char *argv[] ) {
//-----------------------
    return test( argc, argv );
}
//-----------------------
