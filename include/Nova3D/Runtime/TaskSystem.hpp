#pragma once
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

namespace nova3d::runtime {

enum class TaskPriority : uint8_t { High, Normal, Low };
using TaskId = uint64_t;

struct Task {
    TaskId id{};
    TaskPriority priority{TaskPriority::Normal};
    std::function<void()> fn;
    std::vector<TaskId> dependencies;
};

class TaskHandle {
public:
    TaskHandle() = default;
    explicit TaskHandle(std::shared_future<void> f) : m_future(std::move(f)) {}
    bool valid() const { return m_future.valid(); }
    bool isReady() const;
    void wait() const;
private:
    std::shared_future<void> m_future;
};

class TaskQueue {
public:
    void push(Task task);
    std::optional<Task> tryPop();
    size_t size() const;
private:
    mutable std::mutex m_mutex;
    std::queue<Task> m_high, m_normal, m_low;
};

class TaskGraph {
public:
    void addTask(const Task& task);
    bool dependenciesResolved(const Task& task) const;
    void markCompleted(TaskId id);
private:
    std::unordered_map<TaskId, Task> m_tasks;
    std::unordered_map<TaskId, bool> m_completed;
};

class WorkerThread;

class TaskScheduler {
public:
    TaskScheduler();
    ~TaskScheduler();
    void start(size_t workerCount = std::thread::hardware_concurrency());
    void stop();
    TaskHandle schedule(Task task);
    void syncPoint();
private:
    void workerLoop();
    std::atomic<bool> m_running{false};
    std::vector<std::thread> m_threads;
    TaskQueue m_queue;
    TaskGraph m_graph;
    std::mutex m_stateMutex;
    std::condition_variable m_cv;
    std::unordered_map<TaskId, std::promise<void>> m_promises;
    std::atomic<uint64_t> m_nextTaskId{1};
    std::atomic<uint64_t> m_inFlight{0};
};

using ThreadPool = TaskScheduler;

} // namespace nova3d::runtime
