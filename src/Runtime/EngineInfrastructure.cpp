#include <Nova3D/Runtime/EngineInfrastructure.hpp>
namespace nova3d::runtime {
void EventQueue::push(EngineEvent e){std::scoped_lock l(m); q.push(std::move(e));} bool EventQueue::tryPop(EngineEvent& o){std::scoped_lock l(m); if(q.empty()) return false; o=std::move(q.front()); q.pop(); return true;}
size_t EventBus::subscribe(const std::string& n, EventListener l){std::scoped_lock lk(m); auto id=++next; ls[n].push_back({id,std::move(l)}); return id;} void EventBus::emit(const EngineEvent& e){std::scoped_lock lk(m); auto it=ls.find(e.name); if(it==ls.end()) return; for(auto& l:it->second) l.second(e);} void EventBus::pump(EventQueue& q){EngineEvent e; while(q.tryPop(e)) emit(e);} 
void StreamingManager::enqueue(StreamableResource r){std::scoped_lock l(m); q.push(std::move(r));} bool StreamingManager::tryPop(StreamableResource& o){std::scoped_lock l(m); if(q.empty()) return false; o=std::move(q.front()); q.pop(); return true;}
void RenderCommandQueue::push(RenderCommand c){std::scoped_lock l(m); q.push(std::move(c));} bool RenderCommandQueue::tryPop(RenderCommand& o){std::scoped_lock l(m); if(q.empty()) return false; o=std::move(q.front()); q.pop(); return true;}
void AssetChangeMonitor::ingest(FileWatcher& w){ auto p = w.poll(); ev.insert(ev.end(), p.begin(), p.end()); }
void SubsystemRegistry::add(std::shared_ptr<EngineSubsystem> e){s.push_back(std::move(e));} bool SubsystemRegistry::startupAll(){ for(auto& i:s) if(!i->startup()) return false; return true;} void SubsystemRegistry::shutdownAll(){ for(auto it=s.rbegin();it!=s.rend();++it)(*it)->shutdown(); }
}
