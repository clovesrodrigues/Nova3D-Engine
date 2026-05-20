#include <Nova3D/Resources/ResourceSystem.hpp>
#include <algorithm>

namespace nova3d::resources {

static std::string extensionOf(const std::string& path) {
    const auto p = path.find_last_of('.');
    if (p == std::string::npos) return {};
    auto ext = path.substr(p + 1);
    std::ranges::transform(ext, ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext;
}

void ResourceCache::insert(const std::string& key, const std::shared_ptr<IResource>& resource) { m_entries[key] = resource; }
std::shared_ptr<IResource> ResourceCache::find(const std::string& key) const { if (auto it = m_entries.find(key); it != m_entries.end()) return it->second.lock(); return {}; }
void ResourceCache::erase(const std::string& key) { m_entries.erase(key); }
void ResourceCache::clearUnused() { for (auto it = m_entries.begin(); it != m_entries.end();) { if (it->second.expired()) it = m_entries.erase(it); else ++it; } }

void ResourceLoader::registerFactory(const std::string& extension, Factory factory) { m_factories[extension] = std::move(factory); }
std::shared_ptr<IResource> ResourceLoader::create(const std::string& path) const { if (auto it = m_factories.find(extensionOf(path)); it != m_factories.end()) return it->second(path); return {}; }

ResourceManager::ResourceManager(ResourceLoader loader) : m_loader(std::move(loader)) {}
void ResourceManager::setLoader(ResourceLoader loader) { m_loader = std::move(loader); }
std::shared_ptr<IResource> ResourceManager::loadAny(const std::string& path) { if (auto r = m_cache.find(path)) return r; auto r = m_loader.create(path); if (!r) return {}; r->load(); m_cache.insert(path, r); return r; }
void ResourceManager::unload(const std::string& path) { if (auto r = m_cache.find(path)) r->unload(); m_cache.erase(path); }
std::shared_ptr<IResource> ResourceManager::find(const std::string& path) const { return m_cache.find(path); }
void ResourceManager::collectGarbage() { m_cache.clearUnused(); }

} // namespace
