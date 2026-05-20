#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <Nova3D/Resources/Resource.hpp>

namespace nova3d::resources {

class ResourceCache {
public:
    void insert(const std::string& key, const std::shared_ptr<IResource>& resource);
    std::shared_ptr<IResource> find(const std::string& key) const;
    void erase(const std::string& key);
    void clearUnused();
private:
    std::unordered_map<std::string, std::weak_ptr<IResource>> m_entries;
};

class ResourceLoader {
public:
    using Factory = std::function<std::shared_ptr<IResource>(const std::string& path)>;
    void registerFactory(const std::string& extension, Factory factory);
    std::shared_ptr<IResource> create(const std::string& path) const;
private:
    std::unordered_map<std::string, Factory> m_factories;
};

class ResourceManager {
public:
    explicit ResourceManager(ResourceLoader loader = {});
    void setLoader(ResourceLoader loader);
    template <typename T>
    std::shared_ptr<T> load(const std::string& path) {
        auto cached = std::dynamic_pointer_cast<T>(m_cache.find(path));
        if (cached) return cached;
        auto base = m_loader.create(path);
        auto typed = std::dynamic_pointer_cast<T>(base);
        if (!typed) return {};
        typed->load();
        m_cache.insert(path, typed);
        return typed;
    }
    std::shared_ptr<IResource> loadAny(const std::string& path);
    void unload(const std::string& path);
    std::shared_ptr<IResource> find(const std::string& path) const;
    void collectGarbage();
private:
    ResourceLoader m_loader;
    ResourceCache m_cache;
};

} // namespace nova3d::resources
