#include "Heap.h"

Heap::Heap() : _virtAddrStart(nullptr),_Brk(0){
}

void* Heap::init(void* startAddr){
    _Brk = 0;
    _virtAddrStart = (char*)(((uptr)startAddr + 0x1000 -1) / 0x1000 * 0x1000);
    //fprintf(stderr,"Heap base : %p, %p\n",startAddr,_virtAddrStart);
    return _virtAddrStart;
}

iptr Heap::brk(void*addr){
    if(!addr) return (iptr)_virtAddrStart;
    //fprintf(stderr,"brk recieved address %p and virt : %p \n",addr,_virtAddrStart);
    assert((char*)addr >= _virtAddrStart);
    uptr dist = (char*)addr - _virtAddrStart;
    if(dist > _Brk){ // if we should allocate new page
        int nbNewPages = (dist - _Brk + 0x1000 -1)/0x1000;
        char* startAddr = _virtAddrStart + _Brk;
        for(int i = 0 ; i < nbNewPages ; ++ i){
            uptr phy = physmemalloc.alloc();
            paging.createMapping(phy,startAddr + i * 0x1000);
        }
        _Brk+= nbNewPages * 0x1000;
    }
    else if(dist + 0x1000 < _Brk){ // if we should free some page
        int nbPages = (_Brk - dist) / 0x1000;
        paging.freeMappingAndPhy(_virtAddrStart + _Brk - nbPages * 0x1000,nbPages);
        _Brk -= nbPages * 0x1000;
    }
    // else do nothing.
    return 0;
}

Heap kheap;

void* getBrk(){
    return (void*)kheap.brk(nullptr);
}

int brk(void* addr){
    return kheap.brk(addr);
}

