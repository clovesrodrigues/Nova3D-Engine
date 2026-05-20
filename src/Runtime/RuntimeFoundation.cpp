#include <Nova3D/Runtime/RuntimeFoundation.hpp>
#include <chrono>
namespace nova3d::runtime {
void ThreadManager::registerThread(const ThreadContext& c){std::scoped_lock l(m);ctx[c.id]=c;} ThreadContext ThreadManager::current() const {std::scoped_lock l(m); auto it=ctx.find(std::this_thread::get_id()); return it!=ctx.end()?it->second:ThreadContext{};}
TaskHandle AsyncResourceLoader::enqueue(const AsyncResourceRequest& r,std::function<void()> fn){ if(r.cancelled) return {}; Task t; t.priority=r.priority; t.fn=std::move(fn); return m_scheduler.schedule(std::move(t)); }
void ResourceBackgroundLoadQueue::push(ResourceStreamingRequest r){std::scoped_lock l(m);q.push(std::move(r));} bool ResourceBackgroundLoadQueue::tryPop(ResourceStreamingRequest& out){std::scoped_lock l(m); if(q.empty()) return false; out=std::move(q.front()); q.pop(); return true;}
void UploadStagingBuffer::write(const std::vector<std::byte>& bytes){data=bytes;} size_t UploadStagingBuffer::size() const{return data.size();}
void GPUUploadQueue::enqueue(DeferredUploadRequest r){std::scoped_lock l(m); q.push(std::move(r));} bool GPUUploadQueue::tryPop(DeferredUploadRequest& out){std::scoped_lock l(m); if(q.empty()) return false; out=std::move(q.front()); q.pop(); return true;}
void Profiler::begin(const std::string& n){std::scoped_lock l(m); a[n]={double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())};}
void Profiler::end(const std::string& n){std::scoped_lock l(m); auto it=a.find(n); if(it==a.end()) return; auto now=double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()); double ms=(now-it->second.t)/1000.0; auto& st=s[n]; st.lastMs=ms; st.maxMs=std::max(st.maxMs,ms); st.avgMs=st.avgMs==0?ms:(st.avgMs*0.9+ms*0.1); a.erase(it);} TimingStats Profiler::stats(const std::string& n) const {std::scoped_lock l(m); auto it=s.find(n); return it!=s.end()?it->second:TimingStats{};}
EngineStatistics RuntimeStats::snapshot() const {std::scoped_lock l(m); return stats;} void RuntimeStats::set(EngineStatistics e){std::scoped_lock l(m); stats=e;}
}
