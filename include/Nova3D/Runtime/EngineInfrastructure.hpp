#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
namespace nova3d::runtime {
struct EngineEvent { std::string name; std::string payload; };
using EventListener = std::function<void(const EngineEvent&)>;
class EventQueue { public: void push(EngineEvent); bool tryPop(EngineEvent&); private: std::mutex m; std::queue<EngineEvent> q;};
class EventBus { public: size_t subscribe(const std::string&, EventListener); void emit(const EngineEvent&); void pump(EventQueue&); private: std::mutex m; size_t next{}; std::unordered_map<std::string,std::vector<std::pair<size_t,EventListener>>> ls;};
class EventDispatcher { public: explicit EventDispatcher(EventBus& b):bus(b){} void dispatch(const EngineEvent& e){bus.emit(e);} private: EventBus& bus;};

enum class StreamingPriority{Critical,High,Normal,Low}; struct StreamingChunk{int id{};}; struct StreamingRegion{int id{}; std::vector<StreamingChunk> chunks;}; struct StreamableResource{std::string id; StreamingPriority priority{StreamingPriority::Normal};};
class StreamingManager { public: void enqueue(StreamableResource); bool tryPop(StreamableResource&); private: std::mutex m; std::queue<StreamableResource> q;};

struct RenderCommand { std::function<void()> execute;}; class RenderCommandQueue { public: void push(RenderCommand); bool tryPop(RenderCommand&); private: std::mutex m; std::queue<RenderCommand> q;}; struct RenderFrameContext { uint64_t frameIndex{}; }; using RendererSubmissionQueue = RenderCommandQueue;

struct FileWatchEvent { std::string path; std::string action;}; class FileWatcher { public: virtual ~FileWatcher()=default; virtual std::vector<FileWatchEvent> poll()=0;}; class AssetChangeMonitor{ public: void ingest(FileWatcher&); const std::vector<FileWatchEvent>& events() const{return ev;} private: std::vector<FileWatchEvent> ev;};

class EngineSubsystem { public: virtual ~EngineSubsystem()=default; virtual const char* name() const =0; virtual bool startup(){return true;} virtual void shutdown(){} };
class SubsystemRegistry { public: void add(std::shared_ptr<EngineSubsystem>); bool startupAll(); void shutdownAll(); private: std::vector<std::shared_ptr<EngineSubsystem>> s;};
class EngineContext { public: SubsystemRegistry subsystems; EventBus events; };
class ServiceLocator { public: template<typename T> void provide(std::shared_ptr<T> v){ services[typeid(T)] = v; } template<typename T> std::shared_ptr<T> get() const { auto it=services.find(typeid(T)); return it==services.end()?nullptr:std::static_pointer_cast<T>(it->second);} private: std::unordered_map<std::type_index,std::shared_ptr<void>> services;};
}
