#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <Nova3D/Resources/Resource.hpp>

namespace nova3d::assets {
class IAssetImporter { public: virtual ~IAssetImporter() = default; virtual bool canImport(const std::string& extension) const = 0; virtual std::shared_ptr<resources::IResource> importAsset(const std::string& path) = 0; };
class ImporterRegistry {
public:
    using ImporterFactory = std::function<std::shared_ptr<IAssetImporter>()>;
    void registerImporter(const std::string& extension, ImporterFactory importer);
    std::shared_ptr<IAssetImporter> createImporter(const std::string& extension) const;
private:
    std::unordered_map<std::string, ImporterFactory> m_importers;
};
}
