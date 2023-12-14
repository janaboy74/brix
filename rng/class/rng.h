#include <cinttypes>
#include <ctime>
#include <memory>

using namespace std;

//-----------------------
class rng {
//-----------------------
public:
    typedef uint32_t rngType;
    rngType hash;
protected:
    void flip();
public:
    rng( rngType initval = clock() );
    ~rng();
    void init( rngType initval = clock() );
    void initRnd();
    rngType getRnd();
    rngType getRnd( rngType min, rngType max );
};

//-----------------------
extern int32_t get_rnd( int32_t min = 0, int32_t max = 0 );
//-----------------------

//-----------------------
class hashing : public rng {
//-----------------------
protected:
    typedef uint8_t hashType;
    shared_ptr<hashType[]> currentHash;
    size_t hashLength;
public:
    hashing( const char *hash_init = nullptr, size_t hashLength = 16 );
    void init( const char *hash_init, int count = 1 );
    void resize( const char *hash_init = nullptr, size_t hashLength = 16 );
    void genHash( const char *src, int count = 1 );
    void genHash( const void *src, size_t srcLength, int count = 1 );
    void addHash( const char *src, int count = 1 );
    void addHash( const void *src, size_t srcLength, int count = 1 );
    template<class T> T static singlehex( T ch, bool uppercase = false );
    uint16_t static toHex( uint8_t ch, bool uppercase = false );
    string toHexString( bool uppercase = false, bool prefix = false );
    void printHash( bool prefix = false, bool uppercase = false );
};

