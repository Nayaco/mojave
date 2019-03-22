#ifndef MOJAVE_MEM_INDEX
#define MOJAVE_MEM_INDEX 1

#include <map>
#include <utility>
#include "stemmer/types.h"
// index pod use time33 as the hash value
struct index_pod {
    Uint32 _;
    size_t len;
    index_pod *_n;

    index_pod();
    index_pod(const unsigned char *Src, uint32_t _len);
    ~index_pod();

    void set_(const unsigned char *Src, uint32_t _len);
    Uint32 get_() const;
};

// main index use time31 as the hash value
class memIndex {
private:
    std::map< Uint32, index_pod* > _;
    size_t totSize;
public:
    /// default contructor
    /// _ / totSize is empty
    memIndex();

    /// copy constructor
    memIndex(const memIndex &index);

    /// clear the index as initial
    void clear();

    /// insert into a index with return value means existance
    uint32_t insertIndex(const unsigned char *Src, uint32_t _len);

    /// check a sample is in the Index and its hash-value
    std::tuple<bool, Uint32, Uint32> searchIndex(const unsigned char *Src, uint32_t _len);
};


#endif //MOJAVE_MEM_INDEX_HPP
