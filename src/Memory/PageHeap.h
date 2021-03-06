#ifndef PAGEHEAP_H
#define PAGEHEAP_H

#include"../utility.h"
#include"../Bitset.h"




class PageHeap{
    Bitset _bitset;
    void* ialloc(u64 phy);
    void* ialloc(u64 phy,uint nb);
public :
    PageHeap();
    void init();
    template<typename T>
    T* alloc(u64 phy){
        return reinterpret_cast<T*>(ialloc(phy));
    }
    template<typename T>
    T* alloc(u64 phy,uint nb){
        return reinterpret_cast<T*>(ialloc(phy,nb));
    }

    void free(void* virt);
    void free(void* virt, uint nb);

};

extern PageHeap pageHeap;

#endif
