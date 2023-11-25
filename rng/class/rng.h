#include <cinttypes>
#include <ctime>

using namespace std;

//-----------------------
class rng {
//-----------------------
    uint32_t hash;
    void flip();
public:
    rng( int32_t initval = clock() );
    ~rng();
    void init( int32_t initval = clock() );
    void initRnd();
    int32_t getRnd();
    int32_t getRnd( int32_t min, int32_t max );
};

extern int32_t get_rnd( int32_t min = 0, int32_t max = 0 );
