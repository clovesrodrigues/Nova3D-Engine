#pragma once

#include <Nova3D/Animation/AnimationClip.hpp>
#include <Nova3D/Animation/Skeleton.hpp>
#include <Nova3D/Scene/MeshData.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nova::asset {

struct NImportOptions {
    bool triangulate = true;
    bool generateNormals = true;
    bool generateTangents = true;
    bool flipUVs = false;
    bool optimizeMeshes = true;
    bool joinIdenticalVertices = true;
    bool validateDataStructure = true;
};

struct NImportReport {
    std::string sourcePath;
    std::string detectedExtension;
    bool usedAssimpFallback = false;
    uint32_t nodeCount = 0;
    uint32_t meshCount = 0;
    uint32_t materialCount = 0;
    uint32_t animationCount = 0;
    uint32_t skeletonCount = 0;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    bool success = false;
};

struct NMaterial {
    std::string name;
    std::string albedoTexturePath;
    std::string normalTexturePath;
    std::string metallicRoughnessTexturePath;
};

using NSubMesh = nova3d::scene::MeshSection;

struct NMesh {
    std::string name;
    std::vector<nova3d::scene::MeshVertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<NSubMesh> submeshes;
    nova3d::math::AABB bounds{};
};

using NSkeleton = nova3d::animation::Skeleton;
using NAnimationClip = nova3d::animation::AnimationClip;

struct NModelData {
    std::vector<NMesh> meshes;
    std::vector<NMaterial> materials;
    std::vector<NSkeleton> skeletons;
    std::vector<NAnimationClip> animationClips;
};

struct NModelImportResult {
    NModelData data;
    NImportReport report;
};

class NModelImporter {
public:
    NModelImporter();
    NModelImportResult importFromFile(const std::string& path, const NImportOptions& options = {}) const;
};

} // namespace nova::asset
