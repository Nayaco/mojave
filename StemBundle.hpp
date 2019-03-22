#ifndef MOJAVE_STEM_BUNDLE
#define MOJAVE_STEM_BUNDLE 1

#include "stemmer/runtime/api.h"
#include "memIndex.hpp"
#include <cstdint>
#include <vector>
#include <tuple>
#include <map>

typedef std::vector<uint64_t> WordIndexList;
struct stemPod
{
    SN_env *_;
    uint32_t count;
    WordIndexList index;
};

struct stopPod
{
    uint64_t hash;
    uint32_t len;
};

class StemBundle {
private:
    SN_env *_;

    std::map<uint64_t, stemPod> envList;
    std::map<uint64_t, stopPod> stopList;

    uint32_t size;
    uint32_t docNum;
    uint32_t stopSize;
    memIndex index;



    /// return true if in stopwordlist
    bool stopFilter(const char *str, uint32_t len);
public:
    StemBundle();

    virtual ~StemBundle();

    /// return 1 if size is 0
    uint32_t Empty();

    ///return size
    uint32_t Size();

    virtual void AddStop(const char *str, uint32_t len);

    virtual uint32_t Stem(const char *str);

    /// iterator
    class iterator {
    private:
        std::map<uint64_t, stemPod>::iterator iter, endPoint;
    public:
        iterator();

        explicit iterator(std::map<uint64_t, stemPod>::iterator iter,
                          std::map<uint64_t, stemPod>::iterator end);

        iterator& operator ++();

        iterator operator ++(int);

        std::tuple<uint64_t, char*, uint32_t, WordIndexList*> get();

        bool end();
    };

    virtual iterator begin();

    virtual iterator end();
};


#endif //MOJAVE_STEM_BUNDLE_HPP
