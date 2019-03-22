#include "memIndex.hpp"
#include "stemmer/hash.h"
index_pod::index_pod(): _(0), len(0){
}
index_pod::index_pod(const unsigned char *Src, uint32_t len) : len(len){
    _ = Hash(Src, TIME33, (Uint32)len);
}
index_pod::~index_pod(){
    if(_n != nullptr)delete _n;
}

Uint32 index_pod::get_() const {
    return _;
}

void index_pod::set_(const unsigned char *Src, uint32_t _len) {
    _ = Hash(Src, TIME33, (Uint32)_len);
    len = _len;
}



memIndex::memIndex() : totSize(0) {
    _.clear();
}


memIndex::memIndex(const memIndex &index) : _(index._), totSize(index.totSize) {}

void memIndex::clear() {
    _.clear();
    totSize = 0;
}

uint32_t memIndex::insertIndex(const unsigned char *Src, uint32_t len) {
    auto hVal1 = Hash(Src, TIME31, (Uint32)len);
    auto hVal2 = Hash(Src, TIME33, (Uint32)len);
    auto iter = _.find(hVal1);
    if(iter == _.end()) {
        auto newNode = new index_pod(Src, len);
        _.insert(std::make_pair(hVal1, newNode) );
        return 0;
    }else {
        auto iterNode = iter->second;
        for(auto i = 0;
            i < iter->second->len;
            i++, iterNode = iterNode->_n){
            if(iterNode->get_() == hVal2)return 1;
        }

        {
            auto now = iter->second;
            auto newNode = new index_pod(Src, len);
            now->len++;
            newNode->_n = now->_n;
            now->_n = newNode;
        }
        return 0;
    }
}
std::tuple<bool, Uint32, Uint32 > memIndex::searchIndex(const unsigned char *Src, uint32_t len) {
    auto hVal1 = Hash(Src, TIME31, (Uint32)len);
    auto hVal2 = Hash(Src, TIME33, (Uint32)len);
    auto iter = _.find(hVal1);
    if(iter == _.end()) {
        return std::make_tuple(false, hVal1, hVal2);
    }else {
        auto iterNode = iter->second;
        for(auto i = 0;
            i < iter->second->len;
            i++, iterNode = iterNode->_n){
            if(iterNode->get_() == hVal2)return std::make_tuple(true, hVal1, hVal2);
        }
        return std::make_tuple(false, hVal1, hVal1);
    }
}