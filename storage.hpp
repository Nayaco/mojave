#ifndef MOJAVE_STORAGE
#define MOJAVE_STORAGE 1

#include <cstdint>
class storage {
private:
    void * _;
public:
    storage();
    storage(uint32_t size);
    uint32_t write();
    uint32_t read(void *);
};


#endif //MOJAVE_STORAGE
