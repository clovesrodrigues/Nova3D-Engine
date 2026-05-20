#include <Nova3D/Runtime/TaskSystem.hpp>
#include <chrono>
namespace nova3d::runtime {

bool TaskHandle::isReady() const {
    if (!m_future.valid()) return false;
    return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
void TaskHandle::wait() const { if (m_future.valid()) m_future.wait(); }

void TaskQueue::push(Task task) { std::scoped_lock lock(m_mutex); if(task.priority==TaskPriority::High)m_high.push(std::move(task)); else if(task.priority==TaskPriority::Low)m_low.push(std::move(task)); else m_normal.push(std::move(task)); }
std::optional<Task> TaskQueue::tryPop() { std::scoped_lock lock(m_mutex); if(!m_high.empty()){auto t=std::move(m_high.front());m_high.pop();return t;} if(!m_normal.empty()){auto t=std::move(m_normal.front());m_normal.pop();return t;} if(!m_low.empty()){auto t=std::move(m_low.front());m_low.pop();return t;} return std::nullopt; }
size_t TaskQueue::size() const { std::scoped_lock lock(m_mutex); return m_high.size()+m_normal.size()+m_low.size(); }

void TaskGraph::addTask(const Task& task){ m_tasks[task.id]=task; }
bool TaskGraph::dependenciesResolved(const Task& task) const { for(auto dep:task.dependencies){ auto it=m_completed.find(dep); if(it==m_completed.end()||!it->second) return false; } return true; }
void TaskGraph::markCompleted(TaskId id){ m_completed[id]=true; }

TaskScheduler::TaskScheduler() = default;
TaskScheduler::~TaskScheduler(){ stop(); }
void TaskScheduler::start(size_t workerCount){ if(m_running) return; m_running=true; if(workerCount==0) workerCount=1; for(size_t i=0;i<workerCount;++i) m_threads.emplace_back([this]{workerLoop();}); }
void TaskScheduler::stop(){ if(!m_running) return; m_running=false; m_cv.notify_all(); for(auto& t:m_threads) if(t.joinable()) t.join(); m_threads.clear(); }
TaskHandle TaskScheduler::schedule(Task task){ if(task.id==0) task.id = m_nextTaskId.fetch_add(1); std::promise<void> p; auto f=p.get_future().share(); { std::scoped_lock lock(m_stateMutex); m_graph.addTask(task); m_promises.emplace(task.id, std::move(p)); m_queue.push(std::move(task)); m_inFlight.fetch_add(1); } m_cv.notify_one(); return TaskHandle(std::move(f)); }
void TaskScheduler::syncPoint(){ while(m_inFlight.load()>0){ std::this_thread::sleep_for(std::chrono::milliseconds(1)); } }
void TaskScheduler::workerLoop(){ while(m_running){ std::optional<Task> task; { std::unique_lock lock(m_stateMutex); m_cv.wait_for(lock,std::chrono::milliseconds(2),[this]{return !m_running || m_queue.size()>0;}); if(!m_running) return; task = m_queue.tryPop(); }
 if(!task) continue;
 if(!m_graph.dependenciesResolved(*task)){ m_queue.push(std::move(*task)); std::this_thread::yield(); continue; }
 task->fn(); { std::scoped_lock lock(m_stateMutex); m_graph.markCompleted(task->id); auto it=m_promises.find(task->id); if(it!=m_promises.end()){ it->second.set_value(); m_promises.erase(it);} m_inFlight.fetch_sub(1); }
 }
}

} // namespace
