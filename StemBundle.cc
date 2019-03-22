#include "StemBundle.hpp"
#include "stemmer/porter.h"
#include "stemmer/hash.h"
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cassert>

StemBundle::StemBundle(): size(0), docNum(0){
    _ = P_create_env();
    envList.clear();
    stopList.clear();
}

StemBundle::~StemBundle() {
    for(auto stemPodT: envList) {
        P_close_env(stemPodT.second._);
    }
}

uint32_t StemBundle::Empty() {
    return size == 0;
}

uint32_t StemBundle::Size() {
    return size;
}

bool StemBundle::stopFilter(const char *str, uint32_t len) {
    SN_set_current(_, len, (symbol *) str);
    P_stem(_);
    auto hVal1 = Hash(_->p, TIME31, (Uint32)_->l);
    auto hVal2 = Hash(_->p, TIME33, (Uint32)_->l);
    uint64_t hval = 0x00000000FFFFFFFF & hVal1;
    hval <<= 32; hval &= 0xFFFFFFFF00000000; hval |= hVal2;

    auto iter = stopList.find(hval);
    if(iter == stopList.end()) return false;
        else return true;
}

void StemBundle::AddStop(const char *str, uint32_t len) {
    SN_set_current(_, len, (symbol *) str);
    P_stem(_);
    auto hVal1 = Hash(_->p, TIME31, (Uint32)_->l);
    auto hVal2 = Hash(_->p, TIME33, (Uint32)_->l);
    uint64_t hval = 0x00000000FFFFFFFF & hVal1;
    hval <<= 32; hval &= 0xFFFFFFFF00000000; hval |= hVal2;
    stopPod pod = {
        hval,
        len
    };
    stopList[hval] = pod;
}

uint32_t StemBundle::Stem(const char *str) {
    char *strTemp = new char[strlen(str)+1];
    strcpy(strTemp, str);
    uint32_t cur = 0;
    uint32_t num = 0;
    while(1) {
        uint32_t nxt = cur;
        if(strTemp[nxt] == '\0')
            break;
        {
            while(1) {
                if ((strTemp[nxt] < 'A' || strTemp[nxt] > 'Z') && (strTemp[nxt] < 'a' || strTemp[nxt] > 'z'))
                        break;
                    else
                        strTemp[nxt] = tolower(strTemp[nxt]),++nxt;
            }
            if(!stopFilter(strTemp + cur, nxt - cur)) {
                ++num;
                SN_set_current(_, nxt - cur, (symbol *) strTemp + cur);
                P_stem(_);
                uint32_t t31, t33;
                bool isExist;
                std::tie(isExist, t31, t33) = index.searchIndex(_->p, _->l);
                uint64_t pos = t31; pos <<= 32; pos &= 0xFFFFFFFF00000000;pos |= t33;
                    //printf("%20llx %10x %10x | %15s |%3d            ->\n" , pos, t31, t33, _->p, _->l);
                if (isExist) {
                    uint64_t podIndex = docNum;
                    podIndex <<= 32; podIndex += cur;
                    envList[pos].count++;
                    envList[pos].index.push_back(podIndex);
                } else {
                    uint64_t podIndex = docNum;
                    podIndex <<= 32; podIndex += cur;
                    index.insertIndex(_->p, _->l);
                    envList[pos] = stemPod{_, 1, WordIndexList{podIndex}};
                    _ = P_create_env();
                }
            }
        }
        while((strTemp[nxt] < 'A' || strTemp[nxt] > 'Z') && (strTemp[nxt] < 'a' || strTemp[nxt] > 'z') && strTemp[nxt] != '\0')nxt++;
        cur = nxt;
    }
    delete strTemp;
    size += num;
    docNum++;
    return num;
}

StemBundle::iterator::iterator(){}

StemBundle::iterator::iterator(std::map<uint64_t, stemPod>::iterator iter,
                               std::map<uint64_t, stemPod>::iterator end):
                               iter(iter), endPoint(end){}

StemBundle::iterator& StemBundle::iterator::operator++() {
    iter++;
    return *this;
}

StemBundle::iterator StemBundle::iterator::operator++(int) {
    iter++;
    StemBundle::iterator T = *this;
    return T;
}

std::tuple<uint64_t, char*, uint32_t, WordIndexList*> StemBundle::iterator::get(){
    return std::make_tuple(
            iter->first,
            (char*)iter->second._->p,
            iter->second.count,
            &(iter->second.index)
            );
}

bool StemBundle::iterator::end() {
    return iter == endPoint;
}

StemBundle::iterator StemBundle::begin() {
    return StemBundle::iterator(envList.begin(), envList.end());
}

StemBundle::iterator StemBundle::end() {
    return StemBundle::iterator(envList.begin(), envList.end());
}
