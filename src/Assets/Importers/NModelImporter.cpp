#include <Nova3D/Assets/Importers/NModelImporter.hpp>

#include <Nova3D/Core/Logger.hpp>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace {
nova3d::math::AABB computeBounds(const std::vector<nova3d::scene::MeshVertex>& vertices) {
    nova3d::math::AABB b{{1e9F,1e9F,1e9F},{-1e9F,-1e9F,-1e9F}};
    for (const auto& v : vertices) {
        b.min.x = std::min(b.min.x, v.position.x);
        b.min.y = std::min(b.min.y, v.position.y);
        b.min.z = std::min(b.min.z, v.position.z);
        b.max.x = std::max(b.max.x, v.position.x);
        b.max.y = std::max(b.max.y, v.position.y);
        b.max.z = std::max(b.max.z, v.position.z);
    }
    return b;
}

class AssimpBackend final : public nova::asset::NAssimpImporterBackend {
public:
    std::shared_ptr<nova3d::assets::ModelAsset> importModel(const std::string& path, nova::asset::NImportStats* outStats) const override {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);
        if (!scene || !scene->HasMeshes()) {
            nova3d::core::Logger::error(std::string("Assimp failed for ") + path + ": " + importer.GetErrorString());
            return {};
        }

        if (outStats) {
            outStats->nodeCount = scene->mRootNode ? 1u : 0u;
            outStats->meshCount = scene->mNumMeshes;
            outStats->materialCount = scene->mNumMaterials;
            outStats->animationCount = scene->mNumAnimations;
        }

        nova3d::core::Logger::info("Import stats [" + path + "] nodes=" + std::to_string(scene->mRootNode ? 1u : 0u) +
            " meshes=" + std::to_string(scene->mNumMeshes) +
            " materials=" + std::to_string(scene->mNumMaterials) +
            " animations=" + std::to_string(scene->mNumAnimations));

        auto out = std::make_shared<nova3d::assets::ModelAsset>();
        for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
            const aiMesh* mesh = scene->mMeshes[i];
            nova3d::scene::MeshSection sec{};
            sec.indexOffset = static_cast<uint32_t>(out->mesh.indices.size());
            sec.materialSlot = std::to_string(mesh->mMaterialIndex);
            const auto vtxOffset = out->mesh.vertices.size();

            for (unsigned v = 0; v < mesh->mNumVertices; ++v) {
                nova3d::scene::MeshVertex mv{};
                mv.position = {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z};
                if (mesh->HasNormals()) mv.normal = {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z};
                if (mesh->HasTextureCoords(0)) mv.uv = {mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y};
                out->mesh.vertices.push_back(mv);
            }

            for (unsigned f = 0; f < mesh->mNumFaces; ++f) {
                const aiFace& face = mesh->mFaces[f];
                for (unsigned j = 0; j < face.mNumIndices; ++j) {
                    out->mesh.indices.push_back(static_cast<uint32_t>(vtxOffset + face.mIndices[j]));
                }
            }
            sec.indexCount = static_cast<uint32_t>(out->mesh.indices.size()) - sec.indexOffset;
            out->mesh.sections.push_back(sec);
        }

        out->mesh.bounds = computeBounds(out->mesh.vertices);
        return out;
    }
};
}

namespace nova::asset {
NModelImporter::NModelImporter(std::shared_ptr<NAssimpImporterBackend> backend) : m_backend(std::move(backend)) {}
std::shared_ptr<nova3d::assets::ModelAsset> NModelImporter::importFromFile(const std::string& path) const {
    return m_backend ? m_backend->importModel(path, nullptr) : nullptr;
}
std::shared_ptr<nova3d::assets::ModelAsset> NModelImporter::importFromFile(const std::string& path, NImportStats& outStats) const {
    return m_backend ? m_backend->importModel(path, &outStats) : nullptr;
}
std::shared_ptr<NAssimpImporterBackend> createDefaultAssimpImporterBackend() { return std::make_shared<AssimpBackend>(); }
}
