#include "jsonparser.h"
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

using namespace core;

///////////////////////////////////////
jsonItem::jsonItem( const std::shared_ptr<jsonItem> other ) {
///////////////////////////////////////
    operator = ( other );
};

///////////////////////////////////////
jsonItem::jsonItem( const jsonItem &other ) {
///////////////////////////////////////
    operator = ( other );
};

///////////////////////////////////////
void jsonItem::operator = ( const jsonItem &other ) {
///////////////////////////////////////
    values = other.values;
    subItems = other.subItems;
    nodes = other.nodes;
}

///////////////////////////////////////
std::shared_ptr<jsonItem> jsonItem::operator = ( std::shared_ptr<jsonItem> other ) {
///////////////////////////////////////
    values = other->values;
    subItems = other->subItems;
    nodes = other->nodes;
    return other;
}

///////////////////////////////////////
bool jsonItem::empty() {
///////////////////////////////////////
    return !( nodes.size() || subItems.size() || values.size() );
}

///////////////////////////////////////
int json::parse( const char *jsonText ) {
///////////////////////////////////////
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

///////////////////////////////////////
pairvector< uint16_t, corestring > json::nodeToString( pairvector< uint16_t, corestring > &output, std::shared_ptr<jsonItem> item ) {
///////////////////////////////////////
    pairvector< uint16_t, corestring > out;
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

///////////////////////////////////////
std::string json::toString( int identLength ) {
///////////////////////////////////////
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

///////////////////////////////////////
char *json::addString( char *dest, const char *add ) {
///////////////////////////////////////
    while( *add )
        *(dest++) = *( add++ );
    *dest = 0;
    return dest;
}

///////////////////////////////////////
bool json::empty() {
///////////////////////////////////////
    return main->empty();
}
