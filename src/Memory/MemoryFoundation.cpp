#include <Nova3D/Memory/MemoryFoundation.hpp>
#include <algorithm>
namespace nova3d::memory {
void MemoryTracker::onAlloc(size_t n,const std::string& t){std::scoped_lock l(m); s.totalAllocated+=n; s.peakAllocated=std::max(s.peakAllocated,s.totalAllocated); s.tags[t]+=n;} void MemoryTracker::onFree(size_t n,const std::string& t){std::scoped_lock l(m); s.totalAllocated=(s.totalAllocated>n)?(s.totalAllocated-n):0; if(s.tags[t]>=n) s.tags[t]-=n;}
MemoryStats MemoryTracker::snapshot() const {std::scoped_lock l(m); return s;}
MemoryArena::MemoryArena(size_t size):b(size){} void* MemoryArena::data(){return b.data();} size_t MemoryArena::size() const{return b.size();}
LinearAllocator::LinearAllocator(MemoryArena& arena):a(arena){} void* LinearAllocator::allocate(size_t n,size_t){ if(o+n>a.size()) return nullptr; auto* p=static_cast<std::byte*>(a.data())+o; o+=n; return p;} void LinearAllocator::reset(){o=0;}
PoolAllocator::PoolAllocator(size_t chunk,size_t count):buf(chunk*count){ for(size_t i=0;i<count;++i) freeList.push_back(buf.data()+i*chunk);} void* PoolAllocator::allocate(){ if(freeList.empty()) return nullptr; auto* p=freeList.back(); freeList.pop_back(); return p;} void PoolAllocator::free(void* p){ if(p) freeList.push_back(p);} }
