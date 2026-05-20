#pragma once
#include <memory>
#include <string>
#include <vector>
#include <Nova3D/Assets/Importers/MeshImporter.hpp>
#include <Nova3D/Animation/AnimationAssets.hpp>

namespace nova3d::assets {
struct ImportedFBXScene {
    std::vector<std::shared_ptr<ModelAsset>> meshes;
    std::vector<animation::SkeletonAsset> skeletons;
    std::vector<animation::AnimationAsset> animations;
    std::vector<std::string> cameras;
    std::vector<std::string> lights;
};
class IFBXImporterBackend { public: virtual ~IFBXImporterBackend()=default; virtual const char* backendName() const = 0; virtual std::shared_ptr<ImportedFBXScene> importScene(const std::string& path) const = 0; };
class FBXImporter {
public:
    void setBackend(std::shared_ptr<IFBXImporterBackend> backend) { m_backend = std::move(backend); }
    std::shared_ptr<ImportedFBXScene> importFromFile(const std::string& path) const;
private:
    std::shared_ptr<IFBXImporterBackend> m_backend;
};
class StubFBXImporterBackend final : public IFBXImporterBackend { public: const char* backendName() const override { return "stub"; } std::shared_ptr<ImportedFBXScene> importScene(const std::string& path) const override; };
}
