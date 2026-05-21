#include <Nova3D/Assets/Importers/FBXImporter.hpp>
#include <Nova3D/Assets/Importers/NModelImporter.hpp>

namespace nova3d::assets {
std::shared_ptr<ImportedFBXScene> FBXImporter::importFromFile(const std::string& path) const {
    if (!m_backend) return {};
    return m_backend->importScene(path);
}

std::shared_ptr<ImportedFBXScene> StubFBXImporterBackend::importScene(const std::string& path) const {
    nova::asset::NModelImporter importer;
    auto model = importer.importFromFile(path);
    if (!model.report.success || model.data.meshes.empty()) return {};
    auto out = std::make_shared<ImportedFBXScene>();
    auto converted = std::make_shared<ModelAsset>();
    converted->mesh.vertices = model.data.meshes[0].vertices;
    converted->mesh.indices = model.data.meshes[0].indices;
    converted->mesh.sections = model.data.meshes[0].submeshes;
    out->meshes.push_back(std::move(converted));
    return out;
}
}
