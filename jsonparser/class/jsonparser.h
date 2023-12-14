#include <../../corestring/class/corestring.h>
#include <../../corestorage/coremap>
#include <../../corestorage/corevector>

using namespace core;

//--------------------------------
struct jsonItem {
//--------------------------------
    friend class json;
    enum itemType {
        IT_DEFAULT          = 0,
        IT_NODE_ARRAY       = 0x10,
        IT_NODE_ARRAY_SUB   = 0x01 | IT_NODE_ARRAY,
        IT_NODE_ARRAY_LIST  = 0x02 | IT_NODE_ARRAY
    };

    int8_t itemType = IT_DEFAULT;
    coremap<std::string, std::string> values;
    corevector<std::shared_ptr<jsonItem>> subItems;
    coremap<std::string, std::shared_ptr<jsonItem>> nodes;

    jsonItem() {}
    jsonItem( const std::shared_ptr<jsonItem> other );
    jsonItem( const jsonItem &other );
    ~jsonItem() {};
    void operator = ( const jsonItem &other );
    std::shared_ptr<jsonItem> operator = ( std::shared_ptr<jsonItem> other );
    bool isEmpty();
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
        std::shared_ptr<jsonItem> main;
        json() {}
        void parse( const char *jsonText );
        std::string toString( int identLength = 2 );
        char *addString( char *dest, const char *add );
        bool isEmpty();
};
