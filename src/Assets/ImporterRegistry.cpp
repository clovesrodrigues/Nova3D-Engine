#include <Nova3D/Assets/ImporterRegistry.hpp>

namespace nova3d::assets {
void ImporterRegistry::registerImporter(const std::string& extension, ImporterFactory importer) { m_importers[extension] = std::move(importer); }
std::shared_ptr<IAssetImporter> ImporterRegistry::createImporter(const std::string& extension) const { if (auto it = m_importers.find(extension); it != m_importers.end()) return it->second(); return {}; }
}
