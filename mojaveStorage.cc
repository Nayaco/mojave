#include "mojaveStorage.hpp"
#include <random>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "stemmer/runtime/api.h"
#include "stemmer/porter.h"
#include "stemmer/types.h"
#include "stemmer/hash.h"

mojaveStorage::mojaveStorage(): size(0), usize(0), temp(0) {
    std::random_device random;
    std::default_random_engine e1(random());
    std::uniform_int_distribution<int> uniform_dist(0, 35);
    indexName = new char[63];
    for(auto i = 0; i < 63; ++i) {
        auto ran = uniform_dist(e1);
        indexName[i] =  (char)(ran < 26 ? ran + 'z' : ran - 26 + '0');
    }
    M_Index.clear();
    storager.fd = open(indexName, O_RDWR | O_CREAT| O_TRUNC, 0644);
    if(storager.fd < 0)exit(-1);
}

mojaveStorage::mojaveStorage(uint32_t temp): size(0), usize(0), temp(temp) {
    if(temp == 0) {
        std::random_device random;
        std::default_random_engine e1(random());
        std::uniform_int_distribution<int> uniform_dist(0, 35);
        indexName = new char[63];
        for (auto i = 0; i < 63; ++i) {
            auto ran = uniform_dist(e1);
            indexName[i] = (char) (ran < 26 ? ran + 'z' : ran - 26 + '0');
        }
        storager.fd = open(indexName, O_RDWR | O_CREAT| O_TRUNC, 0644);
        if(storager.fd < 0)exit(-1);
    }
    M_Index.clear();
}
mojaveStorage::mojaveStorage(const char *iName): size(0), usize(0), temp(0) {
    indexName = new char[strlen(iName) + 1];
    strcpy(indexName, iName);
    M_Index.clear();
    storager.fd = open(indexName, O_RDWR | O_CREAT| O_TRUNC, 0644);
    if(storager.fd < 0)exit(-1);
}
mojaveStorage::~mojaveStorage() {
    if(temp == 0) {
        uint32_t metaSize = sizeof(M_meta);
        uint32_t mSize = sizeof(MK_node);
        uint32_t pSize = sizeof(P_node);
        uint32_t len = usize * mSize +
                       size * pSize +
                       metaSize;
        lseek(storager.fd, len - 1, SEEK_SET);
        write(storager.fd, "", 1);

        storager.addr = mmap(
                0,
                (size_t) len,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                storager.fd,
                0);
        if (storager.addr == MAP_FAILED) {
            exit(1);
        }
        M_meta meta = {
                0,
                metaSize,
                usize,
                (uint32_t) (usize * mSize + metaSize),
                size
        };
        uint8_t *start = static_cast<uint8_t *>(storager.addr);
        uint8_t *M_start = start + meta.M_nodeoffset;
        uint32_t M_offset = 0;
        uint8_t *P_start = start + meta.P_nodeoffset;
        uint32_t P_offset = 0;
        memcpy(start, &meta, (size_t) metaSize);

        for (auto i: M_Index) {
            MK_node mk = {i.first, (uint32_t) i.second.PosList.size()};
            //printf("%20llx | %d\n", i.first, i.second.PosList.size());
            memcpy(M_start + M_offset, &mk, mSize);
            M_offset += mSize;
            for (auto j: i.second.PosList) {
                //printf("%d\n", P_offset + meta.P_nodeoffset);
                P_node p = {j, 0};
                memcpy(P_start + P_offset, &p, pSize);
                P_offset += pSize;
            }
        }

        if (munmap(storager.addr, (size_t) len) != 0) exit(3);
        truncate(indexName, len);
        storager.addr = nullptr;
        close(storager.fd);
        delete indexName;
    }
}

void mojaveStorage::insert(uint64_t hVal, uint64_t docPos) {
    auto iter = M_Index.find(hVal);
    if(iter == M_Index.end()) {
        ++usize;
        ++size;
        M_Index[hVal] = M_node{ IndexList{docPos} };
    } else {
        ++size;
        M_Index[hVal].PosList.push_back(docPos);
    }
}
void mojaveStorage::insert(uint64_t hVal, IndexList &docPoslist) {
    auto iter = M_Index.find(hVal);
    if(iter  == M_Index.end()) {
        ++usize;
        size += docPoslist.size();
        M_Index[hVal] = M_node{ docPoslist };
    } else {
        size += docPoslist.size();
        M_Index[hVal].PosList.insert(M_Index[hVal].PosList.end(), docPoslist.begin(), docPoslist.end());
    }
}

std::tuple<bool, IndexList*>mojaveStorage::find(const char* str, uint32_t len) {
    SN_env *env = P_create_env();
    SN_set_current(env, len, (const symbol*)str);
    P_stem(env);
    auto hVal1 = (uint32_t)Hash((const unsigned char*)env->p, TIME31, (Uint32)env->l);
    auto hVal2 = (uint32_t)Hash((const unsigned char*)env->p, TIME33, (Uint32)env->l);
    uint64_t hVal = hVal1;hVal <<= 32; hVal &= 0xFFFFFFFF00000000;hVal |= hVal2;
        //printf("%20llx | %15s |%3d            ->" , hVal, env->p, env->l);
    auto iter = M_Index.find(hVal);
    P_close_env(env);
    if(iter == M_Index.end()) {
        IndexList *ans = new IndexList{};
        return std::make_tuple(false, ans);
    } else {
        IndexList *ans = new IndexList{};
        ans->insert(ans->end(), M_Index[hVal].PosList.begin(), M_Index[hVal].PosList.end());
        return std::make_tuple(true, ans);
    }
}

uint32_t mojaveStorage::sync(const char *iName, uint32_t mode) {
    int fd = storager.fd;
    uint32_t metaSize = sizeof(M_meta);
    uint32_t mSize = sizeof(MK_node);
    uint32_t pSize = sizeof(P_node);

    if(mode == SYNC_TO_MEM){

        M_Index.clear();
        M_meta meta = {};
        fd = open(iName, O_RDONLY);
        if(fd < 0)exit(2);
        read(fd, (void*)&meta, metaSize);
        uint32_t len = meta.M_nodeNum * mSize +
                       meta.P_nodeNum * pSize +
                       metaSize;
        lseek(fd, len, SEEK_SET);
        void *addr = mmap(
                NULL,
                (size_t)len,
                PROT_READ,
                MAP_SHARED,
                fd,
                0);
        if(addr == MAP_FAILED)exit(1);
        size = meta.P_nodeNum;
        usize = meta.M_nodeNum;
        uint8_t *start = static_cast<uint8_t *>(addr);
        uint8_t *M_start = start + meta.M_nodeoffset;
        uint32_t M_offset = 0;
        uint8_t *P_start = start + meta.P_nodeoffset;
        uint32_t P_offset = 0;
        for(auto i = 0; i < meta.M_nodeNum; ++i){
            MK_node mk = {};
            memcpy(&mk, M_start + M_offset, mSize);
            M_offset += mSize;
            M_Index.insert(std::pair(mk.hash, M_node{IndexList{}}));
            for(auto j = 0; j < mk.len; ++j) {
                P_node p = {};
                memcpy(&p, P_start + P_offset, pSize);
                P_offset += pSize;
                M_Index[mk.hash].PosList.push_back(p.docPos);

            }
        }

        if(munmap(addr, (size_t)len) != 0) exit(3);
        close(fd);
        return len;
    }

    if(mode == DEFAULT_MODE) {
        fd = open(iName, O_RDWR | O_CREAT | O_TRUNC, 0644);
    }
    if(fd < 0)exit(2);
    uint32_t len = usize * mSize +
                   size * pSize +
                   metaSize;
    lseek(fd, len - 1, SEEK_SET);
    write(fd, "", 1);

    storager.addr = mmap(
            NULL,
            (size_t)len,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd,
            0);
    if(storager.addr == MAP_FAILED)exit(1);
    M_meta meta = {
            0,
            metaSize,
            usize,
            (uint32_t)(usize * mSize + metaSize),
            size
    };
    uint8_t *start = static_cast<uint8_t *>(storager.addr);
    uint8_t *M_start = start + sizeof(M_meta);
    uint32_t M_offset = 0;
    uint8_t *P_start = M_start + usize * mSize;
    uint32_t P_offset = 0;
    memcpy(start, &meta, (size_t)metaSize);
    for(auto i: M_Index){
        MK_node mk = {i.first, (uint32_t)i.second.PosList.size()};
        memcpy(M_start + M_offset, &mk, mSize);
        M_offset += mSize;
        for(auto j: i.second.PosList) {
            P_node p = {j, 0};
            memcpy(P_start + P_offset, &p, pSize);
            P_offset += pSize;
        }
    }
    if(munmap(storager.addr, (size_t)len) != 0) exit(3);
    truncate(indexName, len);
    if(mode == DEFAULT_MODE) {
        close(fd);
    }
    return len;
}

uint32_t mojaveStorage::display() {
    for(auto iter: M_Index) {
        printf("%20llx | ->", iter.first);
        for(auto inside: iter.second.PosList) {
            uint32_t doc = inside >> 32;
            uint32_t pos = inside & 0x00000000FFFFFFFF;
            printf("|%d %d| ", doc, pos);
        }
        printf("\n");
    }
    return 0;
}
