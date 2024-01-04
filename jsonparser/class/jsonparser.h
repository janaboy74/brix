#include <../../corestring/class/corestring.h>
#include <../../corestorage/coremap>
#include <../../corestorage/corevector>
#include <../../corestorage/pairvector>

using namespace core;

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
    jsonItem( const std::shared_ptr<jsonItem> other );
    jsonItem( const jsonItem &other );
    ~jsonItem() {};
    void operator = ( const jsonItem &other );
    std::shared_ptr<jsonItem> operator = ( std::shared_ptr<jsonItem> other );
    bool empty();
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
        int parse( const char *jsonText );
        pairvector< uint16_t, corestring > nodeToString( pairvector< uint16_t, corestring > &output, std::shared_ptr<jsonItem> item );
        std::string toString( int identLength = 2 );
        char *addString( char *dest, const char *add );
        bool empty();
};
