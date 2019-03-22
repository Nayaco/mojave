#include "hash.h"
#include <string.h>

Uint32 TIME17H(unsigned char *Src, Int32 len) {
    Uint32 hash = 5381;
    for(; len >= 8; len -= 8){
        hash = ((hash << 4) + hash) + (*(Src++));
        hash = ((hash << 4) + hash) + (*(Src++));
        hash = ((hash << 4) + hash) + (*(Src++));
        hash = ((hash << 4) + hash) + (*(Src++));
        hash = ((hash << 4) + hash) + (*(Src++));
        hash = ((hash << 4) + hash) + (*(Src++));
        hash = ((hash << 4) + hash) + (*(Src++));
        hash = ((hash << 4) + hash) + (*(Src++));
    }
    switch(len){
        case 7: hash = ((hash << 4) + hash) + (*(Src++)); /*fall*/
        case 6: hash = ((hash << 4) + hash) + (*(Src++)); /*fall*/
        case 5: hash = ((hash << 4) + hash) + (*(Src++)); /*fall*/
        case 4: hash = ((hash << 4) + hash) + (*(Src++)); /*fall*/
        case 3: hash = ((hash << 4) + hash) + (*(Src++)); /*fall*/
        case 2: hash = ((hash << 4) + hash) + (*(Src++)); /*fall*/
        case 1: hash = ((hash << 4) + hash) + (*(Src++));break;
        case 0: break;
    }
    return hash;
}

Uint32 TIME31H(unsigned char *Src, Int32 len) {
    Uint32 hash = 5381;
    for(; len >= 8; len -= 8){
        hash = ((hash << 5) - hash) + (*(Src++));
        hash = ((hash << 5) - hash) + (*(Src++));
        hash = ((hash << 5) - hash) + (*(Src++));
        hash = ((hash << 5) - hash) + (*(Src++));
        hash = ((hash << 5) - hash) + (*(Src++));
        hash = ((hash << 5) - hash) + (*(Src++));
        hash = ((hash << 5) - hash) + (*(Src++));
        hash = ((hash << 5) - hash) + (*(Src++));
    }
    switch(len){
        case 7: hash = ((hash << 5) - hash) + (*Src++); /*fall*/
        case 6: hash = ((hash << 5) - hash) + (*Src++); /*fall*/
        case 5: hash = ((hash << 5) - hash) + (*Src++); /*fall*/
        case 4: hash = ((hash << 5) - hash) + (*Src++); /*fall*/
        case 3: hash = ((hash << 5) - hash) + (*Src++); /*fall*/
        case 2: hash = ((hash << 5) - hash) + (*Src++); /*fall*/
        case 1: hash = ((hash << 5) - hash) + (*Src++);break;
        case 0: break;
    }
    return hash;
}

Uint32 DJBX33A(unsigned char *Src, Int32 len) {
    Uint32 hash = 5381;
    for(; len >= 8; len -= 8){
        hash = ((hash << 5) + hash) + (*(Src++));
        hash = ((hash << 5) + hash) + (*(Src++));
        hash = ((hash << 5) + hash) + (*(Src++));
        hash = ((hash << 5) + hash) + (*(Src++));
        hash = ((hash << 5) + hash) + (*(Src++));
        hash = ((hash << 5) + hash) + (*(Src++));
        hash = ((hash << 5) + hash) + (*(Src++));
        hash = ((hash << 5) + hash) + (*(Src++));
    }
    switch(len){
        case 7: hash = ((hash << 5) + hash) + (*(Src++)); /*fall*/
        case 6: hash = ((hash << 5) + hash) + (*(Src++)); /*fall*/
        case 5: hash = ((hash << 5) + hash) + (*(Src++)); /*fall*/
        case 4: hash = ((hash << 5) + hash) + (*(Src++)); /*fall*/
        case 3: hash = ((hash << 5) + hash) + (*(Src++)); /*fall*/
        case 2: hash = ((hash << 5) + hash) + (*(Src++)); /*fall*/
        case 1: hash = ((hash << 5) + hash) + (*(Src++));break;
        case 0: break;
    }
    return hash;
}

Uint32 TIME65H(unsigned char *Src, Int32 len) {
    Uint32 hash = 5381;
    for(; len >= 8; len -= 8){
        hash = ((hash << 6) + hash) + (*Src++);
        hash = ((hash << 6) + hash) + (*Src++);
        hash = ((hash << 6) + hash) + (*Src++);
        hash = ((hash << 6) + hash) + (*Src++);
        hash = ((hash << 6) + hash) + (*Src++);
        hash = ((hash << 6) + hash) + (*Src++);
        hash = ((hash << 6) + hash) + (*Src++);
        hash = ((hash << 6) + hash) + (*Src++);
    }
    switch(len){
        case 7: hash = ((hash << 6) + hash) + (*Src++); /*fall*/
        case 6: hash = ((hash << 6) + hash) + (*Src++); /*fall*/
        case 5: hash = ((hash << 6) + hash) + (*Src++); /*fall*/
        case 4: hash = ((hash << 6) + hash) + (*Src++); /*fall*/
        case 3: hash = ((hash << 6) + hash) + (*Src++); /*fall*/
        case 2: hash = ((hash << 6) + hash) + (*Src++); /*fall*/
        case 1: hash = ((hash << 6) + hash) + (*Src++);break;
        case 0: break;
    }
    return hash;
}

// The router
Uint32 Hash(const unsigned char *Src, Int32 method, Uint32 len) {
    Uint32 hash = 0;
    switch(method){
        case TIME31:{
            hash = TIME31H(Src, len);
            break;
        } 
        case TIME33:{
            hash = DJBX33A(Src, len);
            break;
        }
        case TIME17:{
            hash = TIME17H(Src, len);
            break;
        }
        case TIME65:{
            hash =  TIME65H(Src, len);
            break;
        }
    }
    return hash;
}

