#pragma once

#include <memory>
#include <string>
#include <Nova3D/Assets/Importers/MeshImporter.hpp>

namespace nova::asset {

class NAssimpImporterBackend {
public:
    virtual ~NAssimpImporterBackend() = default;
    virtual std::shared_ptr<nova3d::assets::ModelAsset> importModel(const std::string& path) const = 0;
};

class NModelImporter {
public:
    explicit NModelImporter(std::shared_ptr<NAssimpImporterBackend> backend);
    std::shared_ptr<nova3d::assets::ModelAsset> importFromFile(const std::string& path) const;
private:
    std::shared_ptr<NAssimpImporterBackend> m_backend;
};

std::shared_ptr<NAssimpImporterBackend> createDefaultAssimpImporterBackend();

} // namespace nova::asset
