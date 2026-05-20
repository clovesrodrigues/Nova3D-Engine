#pragma once
#include <Nova3D/Runtime/TaskSystem.hpp>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
namespace nova3d::runtime {
enum class ThreadType { Main, Worker, RenderPrep, IO, Background };
struct ThreadContext { std::thread::id id{}; ThreadType type{ThreadType::Background}; std::string name; };
class ThreadManager { public: void registerThread(const ThreadContext&); ThreadContext current() const; private: mutable std::mutex m; std::unordered_map<std::thread::id, ThreadContext> ctx;};
struct AsyncResourceRequest { std::string id; TaskPriority priority{TaskPriority::Normal}; std::atomic<bool> cancelled{false}; };
struct ResourceStreamingRequest { std::string id; int regionId{}; int priority{}; };
class AsyncResourceLoader { public: explicit AsyncResourceLoader(TaskScheduler& s):m_scheduler(s){} TaskHandle enqueue(const AsyncResourceRequest&, std::function<void()> fn); private: TaskScheduler& m_scheduler;};
class ResourceBackgroundLoadQueue { public: void push(ResourceStreamingRequest r); bool tryPop(ResourceStreamingRequest& out); private: std::mutex m; std::queue<ResourceStreamingRequest> q;};
struct GPUUploadTask { std::string resourceId; std::vector<std::byte> payload; };
struct DeferredUploadRequest { GPUUploadTask task; TaskPriority priority{TaskPriority::Normal}; };
class UploadStagingBuffer { public: void write(const std::vector<std::byte>& bytes); size_t size() const; private: std::vector<std::byte> data;};
class GPUUploadQueue { public: void enqueue(DeferredUploadRequest r); bool tryPop(DeferredUploadRequest& out); private: std::mutex m; std::queue<DeferredUploadRequest> q;};
struct TimingStats { double lastMs{}, avgMs{}, maxMs{}; };
class Profiler { public: void begin(const std::string&); void end(const std::string&); TimingStats stats(const std::string&) const; private: struct Sample{double t=0;}; mutable std::mutex m; std::unordered_map<std::string,TimingStats> s; std::unordered_map<std::string,Sample> a;};
using CPUProfiler = Profiler; class GPUProfiler { public: virtual ~GPUProfiler() = default; virtual void beginGPUSection(const std::string&){}; virtual void endGPUSection(const std::string&){};};
class ProfileScope { public: ProfileScope(Profiler& p,std::string n):m_p(p),m_n(std::move(n)){m_p.begin(m_n);} ~ProfileScope(){m_p.end(m_n);} private: Profiler& m_p; std::string m_n;};
struct FrameStatistics { float fps{}, frameTimeMs{}; uint64_t tasksExecuted{}; };
struct RenderStatistics { uint32_t drawCalls{}, triangles{}; };
struct StreamingStatistics { uint64_t queuedRequests{}, completedRequests{}; };
struct EngineStatistics { FrameStatistics frame; RenderStatistics render; StreamingStatistics streaming; size_t memoryBytes{}; };
class RuntimeStats { public: EngineStatistics snapshot() const; void set(EngineStatistics e); private: mutable std::mutex m; EngineStatistics stats{};};
}
