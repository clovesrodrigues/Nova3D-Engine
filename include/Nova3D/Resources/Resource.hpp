#pragma once
#include <memory>
#include <string>
#include <unordered_map>

namespace nova3d::resources {

enum class ResourceState { Unloaded, Loading, Loaded, Failed };

class IResource {
public:
    IResource(std::string id, std::string path) : m_id(std::move(id)), m_path(std::move(path)) {}
    virtual ~IResource() = default;
    const std::string& id() const { return m_id; }
    const std::string& path() const { return m_path; }
    ResourceState state() const { return m_state; }
    const std::unordered_map<std::string, std::string>& metadata() const { return m_metadata; }
    void setMetadata(std::string key, std::string value) { m_metadata[std::move(key)] = std::move(value); }
    virtual bool load() = 0;
    virtual void unload() = 0;
    virtual void reload() { unload(); load(); }
protected:
    void setState(ResourceState state) { m_state = state; }
private:
    std::string m_id;
    std::string m_path;
    ResourceState m_state = ResourceState::Unloaded;
    std::unordered_map<std::string, std::string> m_metadata;
};

template <typename T>
class ResourceHandle {
public:
    ResourceHandle() = default;
    explicit ResourceHandle(std::shared_ptr<T> ptr) : m_ptr(std::move(ptr)) {}
    T* operator->() const { return m_ptr.get(); }
    T& operator*() const { return *m_ptr; }
    explicit operator bool() const { return static_cast<bool>(m_ptr); }
    std::shared_ptr<T> lock() const { return m_ptr; }
private:
    std::shared_ptr<T> m_ptr;
};

} // namespace nova3d::resources
