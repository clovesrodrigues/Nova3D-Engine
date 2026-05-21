#pragma once

#include <Nova3D/Animation/AnimationClip.hpp>
#include <Nova3D/Animation/Skeleton.hpp>
#include <Nova3D/Scene/MeshData.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nova::asset {

struct NImportStats {
    std::uint32_t nodeCount = 0;
    std::uint32_t meshCount = 0;
    std::uint32_t materialCount = 0;
    std::uint32_t animationCount = 0;
};

class NAssimpImporterBackend {
public:
    virtual ~NAssimpImporterBackend() = default;
    virtual std::shared_ptr<nova3d::assets::ModelAsset> importModel(const std::string& path, NImportStats* outStats) const = 0;
};

class NModelImporter {
public:
    explicit NModelImporter(std::shared_ptr<NAssimpImporterBackend> backend);
    std::shared_ptr<nova3d::assets::ModelAsset> importFromFile(const std::string& path) const;
    std::shared_ptr<nova3d::assets::ModelAsset> importFromFile(const std::string& path, NImportStats& outStats) const;
private:
    std::shared_ptr<NAssimpImporterBackend> m_backend;
};

} // namespace nova::asset
