#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
namespace nova3d::memory {
struct MemoryStats { size_t totalAllocated{}, peakAllocated{}; std::unordered_map<std::string,size_t> tags; };
class MemoryTracker { public: void onAlloc(size_t,const std::string&); void onFree(size_t,const std::string&); MemoryStats snapshot() const; private: mutable std::mutex m; MemoryStats s;};
class MemoryArena { public: explicit MemoryArena(size_t size); void* data(); size_t size() const; private: std::vector<std::byte> b;};
class LinearAllocator { public: explicit LinearAllocator(MemoryArena&); void* allocate(size_t,size_t align=alignof(std::max_align_t)); void reset(); private: MemoryArena& a; size_t o{};};
using StackAllocator = LinearAllocator; using FrameAllocator = LinearAllocator; class PoolAllocator { public: PoolAllocator(size_t,size_t); void* allocate(); void free(void*); private: std::vector<std::byte> buf; std::vector<void*> freeList;};
}
