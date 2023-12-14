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
#include <../../corestring/class/corestring.h>
#include <../../corestorage/corevector>
#include <../../corestorage/pairvector>
#include <../../corestorage/coremap>
#include <jsonparser.h>

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
bool jsonItem::isEmpty() {
///////////////////////////////////////
    return !( nodes.size() || subItems.size() || values.size() );
}

///////////////////////////////////////
void json::parse( const char *jsonText ) {
///////////////////////////////////////
    std::vector<std::shared_ptr<jsonItem>> node;
    std::vector<std::string> text;
    std::vector<char> stack;
    std::string str;

    main = std::make_shared<jsonItem>();
    if( !jsonText || !strlen( jsonText ))
        return;

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
        if( "null" == lastNode ) {
            text.erase( text.begin() );
            lastNode = text.front();
            continue;
        } else if( "[" == lastNode ) {
            stack.push_back( *text.front().begin() );
            std::shared_ptr<jsonItem> newItem = std::make_shared<jsonItem>();
            if( name.size() ) {
                stack.push_back( '.' );
                std::shared_ptr<jsonItem> newItem = std::make_shared<jsonItem>();
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
            text.erase( text.begin() );
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
            std::cerr << "Parser error / invalid json source\n";
            break;
        }
    }
    if( !( main->itemType &= jsonItem::IT_NODE_ARRAY ) && main->subItems.size() )
        main = *main->subItems.begin();
}

///////////////////////////////////////
std::string json::toString( int identLength ) {
///////////////////////////////////////
    std::string itentString;
    itentString.resize( identLength );
    memset( &*itentString.begin(), ' ', itentString.length() );

    std::vector< std::shared_ptr<jsonItem>> allNodes; // to hold shared_ptr-s
    pairvector< uint16_t, corestring > output;
    pairvector< corestring, std::shared_ptr<jsonItem>> nodeStack;
    std::vector< uint16_t > nodeOutStack;

    std::string name;
    std::string ident;

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
                        output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, nodeStack.back().second->isEmpty() ? "]" : "]," );
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
                    if( !curJson->isEmpty() ) {
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
                if( pos == std::string::npos )
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
            if( curJson->isEmpty() ) {
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
                if( !curJson->isEmpty() )
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
                            output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, curJson->isEmpty() ? "]" : "]," );
                        }
                    }
                } else {
                    output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, curJson->isEmpty() ? "}" : "}," );
                }
            } else {
                output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, "}" );
            }
            continue;
        }
        if( curJson->isEmpty() ) {
            nodeStack.pop_back();
            nodeOutStack.pop_back();
            if( curJson->itemType &= jsonItem::IT_NODE_ARRAY ) {
                if( nodeStack.size() ) {
                    curJson = nodeStack.back().second;
                    output.push_back( BT_DECREASE_IDENT | BT_USE_IDENT | BT_ITEM | BT_NEWLINE, curJson->isEmpty() ? "]" : "]," );
                }
            } else
                output.push_back( BT_USE_IDENT | BT_DECREASE_IDENT | BT_NODE_ARRAY_STOP | BT_NEWLINE | BT_ITEM, "}" );
            break;
        }
    }
    std::string outputstring;
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

///////////////////////////////////////
char *json::addString( char *dest, const char *add ) {
///////////////////////////////////////
    while( *add )
        *(dest++) = *( add++ );
    *dest = 0;
    return dest;
}

///////////////////////////////////////
bool json::isEmpty() {
///////////////////////////////////////
    return main->isEmpty();
}
