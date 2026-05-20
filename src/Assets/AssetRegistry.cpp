#include <Nova3D/Assets/AssetRegistry.hpp>

namespace nova3d::assets {

void AssetRegistry::registerImporter(const std::string& extension, ImporterFactory factory) {
    importers_[extension] = std::move(factory);
}

bool AssetRegistry::hasImporter(const std::string& extension) const {
    return importers_.contains(extension);
}

} // namespace nova3d::assets
