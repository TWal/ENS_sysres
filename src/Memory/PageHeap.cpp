#include"PageHeap.h"
#include"PhysicalMemoryAllocator.h"
#include"Paging.h"

void* const phBitset  =(void*) (- 0xB0000000ll - 0x2000ll);
u8* const base = (u8*)(-0xC0000000ll - 0x1000);


void* PageHeap::ialloc(u64 phy){
    int i = _bitset.bsf();
    _bitset.unset(i);
    assert(i != -1);
    //printf("found %d",i);
    u8* addr = base - i * 0x1000;
    printf("page Heap alloc at %p\n",addr);
    paging.createMapping(phy,addr);
    return addr;
}
void* PageHeap::ialloc(u64 phy,int nb){
    int i = _bitset.largeBsf(nb);
    assert(i != -1);
    for(int i = 0; i < nb ; ++i)
    _bitset.unset(i);
    //printf("found %d",i);
    u8* addr = base - i * 0x1000;
    printf("page Heap alloc at %p of size %d\n",addr,nb);
    paging.createMapping(phy,addr,nb);
    return addr;

}
PageHeap::PageHeap(){
}
void PageHeap::init(){
    paging.createMapping(physmemalloc.alloc(),phBitset);
    _bitset.init(phBitset,0x1000);
    _bitset.fill();
    _bitset[0] = false;
}
void PageHeap::free(void* virt){
    size_t pos = uptr(base - (u8*)virt) / 0x1000;
    //printf("base : %p, virt : %p, pos : %d",base,virt,pos);
    assert(pos < 0x1000);
    paging.freeMapping(virt);
    _bitset.set(pos);
}

PageHeap pageHeap;
