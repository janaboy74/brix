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

using namespace std;

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
    set<size_t> removed;
    set<size_t> inserted;
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
template <class V> struct corevector : public vector<V> {
//--------------------------------
    shared_ptr<corevectormodificator> vectormodificator;
public:
    corevector() : vector<V>() {}
    corevector( const size_t s, const V v ) {
        vector<V>::resize( s );
        for( auto &var : *this )
            var = v;
    }
    V &operator[]( const size_t index ) {
        static V dummy;
        if( index < this->size() )
            return vector<V>::operator[]( index );
        return dummy;
    }
    corevectormodificator &modificator() {
        if( !vectormodificator.get() )
            vectormodificator = make_shared<corevectormodificator>();
        return *vectormodificator.get();
    }
    void update() {
        if( !vectormodificator.get() )
            return;
        for( auto &item : vectormodificator->removed ) {
            vector<V>::remove( item );
        }
        for( auto &item : vectormodificator->inserted ) {
            vector<V>::insert( item );
        }
    }
};

template <class I, class V> class coremapmodificator {
    set< I> removed;
    map< I, V> inserted;
    template<class X, class Y> friend class coremap;
public:
    void insert( const pair< I, V > item ) {
        insert( item.first, item.second );
    }
    void insert( const I item, const V val ) {
        if( removed.find( item ) != removed.end() )
            removed.erase( item );
        inserted.insert( pair<I, V>( item, val ));
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
template <class I, class V> class coremap : public map<I, V> {
//--------------------------------
    shared_ptr<coremapmodificator<I, V>> mapmodificator;
public:
    V &operator[]( const I item ) {
        static V dummy;
        if( contains( item ))
            return map<I, V>::operator[]( item );
        return dummy;
    }
    void insert( const I item, const V val ) {
        map<I, V>::insert( pair<I, V>( item, val ));
    }
    bool contains( const I item ) const {
        return this->find( item ) != this->end();
    }
    coremapmodificator<I, V> &modificator() {
        if( !mapmodificator.get() )
            mapmodificator = make_shared<coremapmodificator<I, V>>();
        return *mapmodificator.get();
    }
    void update( coremapmodificator<I, V> *modificator = nullptr ) {
        auto &executor = modificator ? ( *modificator) : ( *mapmodificator.get() );
        if( !&executor )
            return;
        for( auto &item : executor.removed ) {
            map<I,V>::erase( item );
        }
        for( auto &item : executor.inserted ) {
            map<I,V>::insert( pair<I, V>( item.first, item.second ));
        }
        if( !modificator )
            mapmodificator->cleanup();
    }
};

//--------------------------------
template <class I, class V> struct pairvector : public vector<pair<I,V>> {
//--------------------------------
    pairvector() : vector<pair<I,V>>() {}
    void push_back( const I item, const V val ) {
        vector<pair<I,V>>::push_back( pair<I, V>( item, val ));
    }
    pair<I,V> &operator[]( const size_t index ) {
        static pair<I,V> dummy;
        if( index < this->size() )
            return vector<pair<I,V>>::operator[]( index );
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

    jsonItem() {}
    jsonItem( const shared_ptr<jsonItem> other ) {
        operator = ( other );
    };
    jsonItem( const jsonItem &other ) {
        operator = ( other );
    };
    ~jsonItem() {};
    int8_t itemType = IT_DEFAULT;
    coremap<string, string> values;
    corevector<shared_ptr<jsonItem>> subItems;
    coremap<string, shared_ptr<jsonItem>> nodes;
    void operator = ( const jsonItem &other ) {
        values = other.values;
        subItems = other.subItems;
        nodes = other.nodes;
    }
    shared_ptr<jsonItem> operator = ( shared_ptr<jsonItem> other ) {
        values = other->values;
        subItems = other->subItems;
        nodes = other->nodes;
        return other;
    }
    bool empty() {
        return !( nodes.size() || subItems.size() || values.size() );
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
        BT_NODE_ARRAY_START     = 0x0001,
        BT_NODE_ARRAY_STOP      = 0x0002,
        BT_NODE_START           = 0x0003,
        BT_NODE_STOP            = 0x0004,
        BT_ITEM                 = 0x0005,
        BT_FINAL                = 0x00fe,
    };

public:
        shared_ptr<jsonItem> main;
        json() {}
        int parse( const char *jsonText ) { // return the error code - see below
            vector<shared_ptr<jsonItem>> node;
            vector<string> text;
            vector<char> stack;
            string str;

            main = make_shared<jsonItem>();
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
            shared_ptr<jsonItem> curItem;
            if( !text.size() || text.front() != "[" ) {
                main = make_shared<jsonItem>();
                node.push_back( main );
                curItem = node.back();
            }
            string name, val, lastNode = "";
            char lastStack = 0;
            while( text.size() ) {
                if( stack.size() )
                    lastStack = stack.back();
                if( text.size() )
                    lastNode = text.front();
                if( "null" == lastNode ) {
                    text.erase( text.begin() );
                    lastNode = text.front();
                    continue;
                } else if( "[" == lastNode ) {
                    stack.push_back( *text.front().begin() );
                    shared_ptr<jsonItem> newItem = make_shared<jsonItem>();
                    if( name.size() ) {
                        stack.push_back( '.' );
                        shared_ptr<jsonItem> newItem = make_shared<jsonItem>();
                        curItem->nodes.insert( name, newItem );
                        node.push_back( newItem );
                        curItem = node.back();
                        text.erase( text.begin() );
                        continue;
                    } else {
                        main = newItem;
                    }
                    node.push_back( newItem );
                    curItem = node.back();
                    text.erase( text.begin() );
                } else if( "]" == lastNode ) {
                    curItem->itemType = jsonItem::IT_NODE_ARRAY_SUB;
                    stack.pop_back();
                    node.erase( node.end() );
                    if( !node.empty() ) {
                        return 2; // too much closing brackets
                    }
                    text.erase( text.begin() );
                    if( node.size() )
                        curItem = node.back();
                    else
                        break;
                } else if( "{" == lastNode ) {
                    if( '.' == lastStack ) {
                        shared_ptr<jsonItem> newItem = make_shared<jsonItem>();
                        curItem->subItems.push_back( newItem );
                        node.push_back( newItem );
                        curItem = node.back();
                        stack.push_back( *lastNode.begin() );
                        text.erase( text.begin() );
                        continue;
                    }
                    shared_ptr<jsonItem> newItem = make_shared<jsonItem>();
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
                    if( !node.empty() ) {
                        return 2; // too much closing brackets
                    }
                    curItem = node.back();
                    text.erase( text.begin() );
                } else if( text.size() > 1 && text[1] == ":" ) {
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
        string toString( int identLength = 2 ) {
            string itentString;
            itentString.resize( identLength );
            memset( &*itentString.begin(), ' ', itentString.length() );

            vector< shared_ptr<jsonItem>> allNodes; // to hold shared_ptr-s
            pairvector< uint16_t, corestring > output;
            pairvector< corestring, shared_ptr<jsonItem>> nodeStack;
            vector< uint16_t > nodeOutStack;

            string name;
            string ident;

            allNodes.push_back( main );
            auto curJson = main;
            nodeStack.push_back( "empty", main );

            if( curJson->itemType &= jsonItem::IT_NODE_ARRAY ) {
                nodeOutStack.push_back( BT_NODE_ARRAY_START );
                output.push_back( BT_INCREASE_IDENT | BT_NODE_ARRAY_START | BT_NEWLINE | BT_ITEM, "[" );
            } else {
                nodeOutStack.push_back( BT_NODE_START );
                output.push_back( BT_INCREASE_IDENT | BT_NODE_ARRAY_START | BT_NEWLINE | BT_ITEM, "{" );
            }

            while( nodeStack.size() ) {
                curJson = nodeStack.back().second;
                if( curJson->itemType &= jsonItem::IT_NODE_ARRAY ) {
                    if( curJson->subItems.size() ) {
                        auto &subItems = curJson->subItems;
                        nodeStack.push_back( "array :", subItems.front() );
                        nodeOutStack.push_back( BT_NODE_START );
                        output.push_back( BT_USE_IDENT | BT_INCREASE_IDENT | BT_NODE_START | BT_NEWLINE | BT_ITEM, "{" );
                        subItems.erase(subItems.begin());
                        curJson = nodeStack.back().second;
                    } else {
                        if( nodeOutStack.size() ) {
                            nodeStack.pop_back();
                            nodeOutStack.pop_back();
                        }
                        if( curJson->itemType &= jsonItem::IT_NODE_ARRAY ) {
                            if( nodeStack.size() ) {
                                output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, nodeStack.back().second->empty() ? "]" : "]," );
                                curJson = nodeStack.back().second;
                            } else {
                                output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, "]" );
                                break;
                            }
                        }
                    }
                    continue;
                }
                if( curJson->nodes.size() ) {
                    auto &nodes = curJson->nodes;
                    nodeStack.push_back( nodes.begin()->first, nodes.begin()->second );
                    allNodes.push_back( nodeStack.back().second );
                    output.push_back( BT_USE_IDENT | BT_ITEM, '"' );
                    output.push_back( BT_ITEM, nodes.begin()->first );
                    output.push_back( BT_ITEM, '"' );
                    output.push_back( BT_ITEM, " : " );
                    auto &nextNode = nodes.begin()->second;
                    if( nextNode->itemType &= jsonItem::IT_NODE_ARRAY ) {
                        if( nextNode->subItems.size() ) {
                            nodeOutStack.push_back( BT_NODE_ARRAY_START );
                            output.push_back( BT_INCREASE_IDENT | BT_NODE_ARRAY_START | BT_NEWLINE | BT_ITEM, "[" );
                            if( nextNode->nodes.size() ) {
                                output.push_back( BT_USE_IDENT | BT_INCREASE_IDENT | BT_NODE_START | BT_NEWLINE | BT_ITEM, "{" );
                            }
                        } else {
                            nodeStack.pop_back();
                            if( nextNode->nodes.size() ) {
                                output.push_back( BT_NODE_ARRAY_START | BT_ITEM, "[ " );
                                while( nextNode->nodes.size() ) {
                                    auto firstnode = nextNode->nodes.begin();
                                    output.push_back( BT_ITEM, '"' );
                                    output.push_back( BT_ITEM, firstnode->first );
                                    output.push_back( BT_ITEM, '"' );
                                    nextNode->nodes.erase( firstnode );
                                    if( nextNode->nodes.size() )
                                        output.push_back( BT_ITEM, ", " );
                                }
                                output.push_back( BT_NODE_ARRAY_START | BT_ITEM, " ]" );
                            } else {
                                output.push_back( BT_NODE_ARRAY_START | BT_ITEM, "[]" );
                            }
                            if( !curJson->empty() ) {
                                output.push_back( BT_ITEM, "," );
                            }
                            output.push_back( BT_NEWLINE, "" );
                        }
                        nodes.erase( nodes.begin()->first );
                        continue;
                    }
                    if( !( curJson->itemType &= jsonItem::IT_NODE_ARRAY )) {
                        output.push_back( BT_INCREASE_IDENT | BT_NODE_START | BT_NEWLINE | BT_ITEM, "{" );
                    }
                    nodeOutStack.push_back( BT_NODE_START );
                    nodes.erase( nodes.begin()->first );
                    continue;
                }
                auto &values = curJson->values;
                if( values.size() ) {
                    auto &value = *values.begin();
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
                        if( pos == string::npos )
                            break;
                        realValue.replace( pos, strlen( src ), "\\\"" );
                        pos += strlen( dest );
                    }
                    if(( "false" == realValue || "true" == realValue ) && realValue.length() ) {
                        output.push_back( BT_ITEM, realValue );
                    } else {
                        output.push_back( BT_ITEM, '"' );
                        output.push_back( BT_ITEM, realValue );
                        output.push_back( BT_ITEM, '"' );
                    }
                    values.erase( values.begin() );
                    if( curJson->empty() ) {
                        output.push_back( BT_NEWLINE, "" );
                        output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM, "}" );
                        nodeStack.pop_back();
                        nodeOutStack.pop_back();
                        if( nodeStack.size() ) {
                            curJson = nodeStack.back().second;
                        } else {
                            output.push_back( BT_NEWLINE, "" );
                            break;
                        }
                        if( !curJson->empty() )
                            output.push_back( BT_ITEM, "," );
                        output.push_back( BT_NEWLINE, "" );
                    } else {
                        output.push_back( BT_ITEM | BT_NEWLINE, "," );
                    }
                    continue;
                }
                if(( nodeOutStack.back() & BT_TYPE_MASK ) == BT_NODE_START  ) {
                    nodeStack.pop_back();
                    nodeOutStack.pop_back();
                    if( nodeOutStack.size() ) {
                        curJson = nodeStack.back().second;
                        if(( nodeOutStack.back() & BT_TYPE_MASK ) == BT_NODE_ARRAY_START ) {
                            if( curJson->subItems.size() ) {
                                output.push_back( BT_DECREASE_IDENT | BT_NEWLINE, "" );
                                output.push_back( BT_USE_IDENT | BT_ITEM | BT_NEWLINE, "}, {" );
                            } else {
                                output.push_back( BT_NEWLINE, "" );
                                output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, "}" );
                                nodeStack.pop_back();
                                nodeOutStack.pop_back();
                                if( nodeStack.size() ) {
                                    curJson = nodeStack.back().second;
                                    output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, curJson->empty() ? "]" : "]," );
                                }
                            }
                        } else {
                            output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, curJson->empty() ? "}" : "}," );
                        }
                    } else {
                        output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, "}" );
                    }
                    continue;
                }
                if( curJson->empty() ) {
                    nodeStack.pop_back();
                    nodeOutStack.pop_back();
                    if( curJson->itemType &= jsonItem::IT_NODE_ARRAY ) {
                        if( nodeStack.size() ) {
                            curJson = nodeStack.back().second;
                            output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, curJson->empty() ? "]" : "]," );
                        }
                    } else
                        output.push_back( BT_USE_IDENT | BT_DECREASE_IDENT | BT_NODE_ARRAY_STOP | BT_NEWLINE | BT_ITEM, "}" );
                    break;
                }
            }
            string outputstring;
            char *out = 0;
            bool predict = true;
            size_t size = 0;

            for( auto item = output.begin();; ++item ) {
                if( item == output.end() ) {
                    if( predict ) {
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
    string result;
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
            cout << json.toString().c_str();
        } else
            std::cerr << "Unknown error in json source.\n";
        break;
    }

    cout.flush();
    fflush( stdout );

    return 0;
}

//-----------------------
int main( int argc, char *argv[] ) {
//-----------------------
    return test( argc, argv );
}
//-----------------------
