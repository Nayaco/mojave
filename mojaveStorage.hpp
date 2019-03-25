/**
 * WARNING
 * there's one thing should be clear that
 * Windows could never run this application
 * cauz it use a POSIX-compliant Unix system call
 * IT WILL NEVER WORK
 * SAVE YOUR TIME don't even try to compile thie on a NT system
*/

#ifndef MOJAVE_MOJAVESTORAGE
#define MOJAVE_MOJAVESTORAGE 1

#include <vector>
#include <cstdint>
#include <tuple>
#include <map>
#define MAX_BRANCH 256
enum NodeType {B_NODE_C, D_NODE_C};

#define B_NODE_C 0
#define D_NODE_C 1

typedef std::vector<uint64_t> IndexList;
/// memory part
struct Base_node {
    uint32_t cType;
};
struct B_node_inner {
    uint64_t value;
    uint32_t cType;
    B_node_inner *next;
    Base_node* _;
};
struct B_node: Base_node {
    uint32_t N;
    Base_node *pNode;
    Base_node *minNode;
    B_node_inner *childlist;
    virtual ~B_node();
};

/// mmaped
struct D_node: Base_node {
    uint32_t N;
    Base_node *pNode;
    uint32_t IDList[MAX_BRANCH];
};

/// disk part
struct P_node {
    uint64_t docPos;
    uint32_t next;
};

struct D_meta {
    uint32_t D_metaoffset;
    uint32_t D_nodeoffset;
    uint32_t D_nodeNum;
    uint32_t P_nodeoffset;
    uint32_t P_nodeNum;
};

/// map index
struct M_node {
    IndexList PosList;
};
struct MK_node {
    uint64_t hash;
    uint32_t len;
};
struct M_meta {
    uint32_t M_metaoffset;
    uint32_t M_nodeoffset;
    uint32_t M_nodeNum;
    uint32_t P_nodeoffset;
    uint32_t P_nodeNum;
};
/// stroage properties
struct Storager {
    int fd;
    void *addr;
};
/// main storage engine
#define DEFAULT_MODE 1
#define NEW_FILE     3
#define SYNC_TO_MEM  7
class mojaveStorage {
private:
    /// storager
    char *indexName;
    Storager storager;
    /// size of the indexlist
    uint32_t size;
    uint32_t usize;

    /// the Hash map using TIME33 and TIME31
    std::map<uint64_t, M_node> M_Index;
    uint32_t temp;
public:
    mojaveStorage();
    mojaveStorage(uint32_t temp);
    explicit mojaveStorage(const char *iName);

    /// the destruction contains the storage process
    /// when it goes out of the domain, it will store auto matically
    ~mojaveStorage();

    /// insert and search from the inverted index
    void insert(uint64_t hVal, uint64_t docPos);
    void insert(uint64_t hVal, IndexList &docPoslist);
    std::tuple<bool, IndexList*> find(const char* str, uint32_t len);

    /// this is for consistence storage and sync from the memory
    uint32_t sync(const char *iName, uint32_t mode);
    uint32_t display(); //test
};



#endif //MOJAVE_MOJAVESTORAGE
