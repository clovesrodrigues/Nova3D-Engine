#include <Nova3D/Assets/Importers/FBXImporter.hpp>
#include <Nova3D/Assets/Importers/NModelImporter.hpp>

namespace nova3d::assets {
std::shared_ptr<ImportedFBXScene> FBXImporter::importFromFile(const std::string& path) const {
    if (!m_backend) return {};
    return m_backend->importScene(path);
}

std::shared_ptr<ImportedFBXScene> StubFBXImporterBackend::importScene(const std::string& path) const {
    auto modelBackend = nova::asset::createDefaultAssimpImporterBackend();
    nova::asset::NImportStats stats{};
    auto model = modelBackend->importModel(path, &stats);
    if (!model) return {};
    auto out = std::make_shared<ImportedFBXScene>();
    out->meshes.push_back(model);
    return out;
}
}
