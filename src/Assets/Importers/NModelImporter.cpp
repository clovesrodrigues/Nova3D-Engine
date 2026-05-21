#include <Nova3D/Assets/Importers/NModelImporter.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algorithm>
#include <filesystem>
#include <cctype>
#include <functional>
#include <limits>
#include <unordered_set>

namespace {

using namespace nova::asset;

std::string normalizeExtension(const std::string& path) {
    std::filesystem::path p(path);
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext;
}

nova3d::math::AABB computeBounds(const std::vector<nova3d::scene::MeshVertex>& vertices) {
    if (vertices.empty()) {
        return {};
    }
    nova3d::math::AABB b{{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()},
                 {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()}};
    for (const auto& v : vertices) {
        b.min.x = std::min(b.min.x, v.position.x); b.min.y = std::min(b.min.y, v.position.y); b.min.z = std::min(b.min.z, v.position.z);
        b.max.x = std::max(b.max.x, v.position.x); b.max.y = std::max(b.max.y, v.position.y); b.max.z = std::max(b.max.z, v.position.z);
    }
    return b;
}

class NAssimpImporterBackend final {
public:
    NModelImportResult importModel(const std::string& path, const NImportOptions& options, bool fallbackUsed, const std::string& ext) const {
        NModelImportResult result{};
        result.report.sourcePath = path;
        result.report.detectedExtension = ext;
        result.report.usedAssimpFallback = fallbackUsed;

        Assimp::Importer importer;
        unsigned flags = 0;
        if (options.triangulate) flags |= aiProcess_Triangulate;
        if (options.generateNormals) flags |= aiProcess_GenSmoothNormals;
        if (options.generateTangents) flags |= aiProcess_CalcTangentSpace;
        if (options.flipUVs) flags |= aiProcess_FlipUVs;
        if (options.optimizeMeshes) flags |= aiProcess_OptimizeMeshes;
        if (options.joinIdenticalVertices) flags |= aiProcess_JoinIdenticalVertices;
        if (options.validateDataStructure) flags |= aiProcess_ValidateDataStructure;

        const aiScene* scene = importer.ReadFile(path, flags);
        if (!scene) {
            result.report.errors.emplace_back(std::string("Assimp error: ") + importer.GetErrorString());
            return result;
        }

        result.report.nodeCount = scene->mRootNode ? 1 : 0;
        std::function<void(const aiNode*)> countNodes = [&](const aiNode* node) {
            for (unsigned i = 0; i < node->mNumChildren; ++i) {
                ++result.report.nodeCount;
                countNodes(node->mChildren[i]);
            }
        };
        if (scene->mRootNode) {
            countNodes(scene->mRootNode);
        }

        result.report.materialCount = scene->mNumMaterials;
        for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
            const aiMaterial* mat = scene->mMaterials[i];
            NMaterial outMat{};
            aiString matName;
            if (mat->Get(AI_MATKEY_NAME, matName) == AI_SUCCESS) {
                outMat.name = matName.C_Str();
            }
            aiString texPath;
            if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) outMat.albedoTexturePath = texPath.C_Str();
            if (mat->GetTexture(aiTextureType_NORMALS, 0, &texPath) == AI_SUCCESS) outMat.normalTexturePath = texPath.C_Str();
            if (mat->GetTexture(aiTextureType_UNKNOWN, 0, &texPath) == AI_SUCCESS) outMat.metallicRoughnessTexturePath = texPath.C_Str();
            if (outMat.name.empty()) outMat.name = "material_" + std::to_string(i);
            if (outMat.albedoTexturePath.empty()) result.report.warnings.emplace_back("Material '" + outMat.name + "' missing diffuse texture, using default color.");
            if (outMat.normalTexturePath.empty()) result.report.warnings.emplace_back("Material '" + outMat.name + "' missing normal texture.");
            result.data.materials.push_back(std::move(outMat));
        }

        result.report.meshCount = scene->mNumMeshes;
        for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
            const aiMesh* m = scene->mMeshes[i];
            NMesh mesh{};
            mesh.name = m->mName.C_Str();
            NSubMesh sm{};
            sm.indexOffset = 0;
            sm.vertexOffset = 0;
            sm.materialSlot = std::to_string(m->mMaterialIndex);

            for (unsigned v = 0; v < m->mNumVertices; ++v) {
                nova3d::scene::MeshVertex mv{};
                mv.position = {m->mVertices[v].x, m->mVertices[v].y, m->mVertices[v].z};
                if (m->HasNormals()) mv.normal = {m->mNormals[v].x, m->mNormals[v].y, m->mNormals[v].z};
                else result.report.warnings.emplace_back("Mesh '" + mesh.name + "' has no normals.");
                if (m->HasTextureCoords(0)) mv.uv = {m->mTextureCoords[0][v].x, m->mTextureCoords[0][v].y};
                else result.report.warnings.emplace_back("Mesh '" + mesh.name + "' has no UV0.");
                mesh.vertices.push_back(mv);
            }

            std::unordered_set<std::string> boneNames;
            for (unsigned b = 0; b < m->mNumBones; ++b) {
                boneNames.insert(m->mBones[b]->mName.C_Str());
            }

            for (unsigned f = 0; f < m->mNumFaces; ++f) {
                const aiFace& face = m->mFaces[f];
                if (face.mNumIndices != 3 && options.triangulate) {
                    result.report.warnings.emplace_back("Non-triangle face after triangulation in mesh '" + mesh.name + "'.");
                }
                for (unsigned j = 0; j < face.mNumIndices; ++j) {
                    mesh.indices.push_back(face.mIndices[j]);
                }
            }
            sm.indexCount = static_cast<uint32_t>(mesh.indices.size());
            mesh.bounds = computeBounds(mesh.vertices);
            sm.bounds = mesh.bounds;
            mesh.submeshes.push_back(sm);
            result.data.meshes.push_back(std::move(mesh));

            if (!boneNames.empty()) {
                nova3d::animation::BoneHierarchy hierarchy;
                for (const auto& boneName : boneNames) {
                    nova3d::animation::Bone bone{};
                    bone.name = boneName;
                    hierarchy.addBone(std::move(bone));
                }
                NSkeleton skeleton;
                skeleton.setHierarchy(std::move(hierarchy));
                result.data.skeletons.push_back(std::move(skeleton));
            }
        }

        result.report.animationCount = scene->mNumAnimations;
        for (unsigned i = 0; i < scene->mNumAnimations; ++i) {
            const aiAnimation* anim = scene->mAnimations[i];
            NAnimationClip clip(anim->mName.C_Str());
            if (clip.name().empty()) {
                clip = NAnimationClip("animation_" + std::to_string(i));
            }
            const float tps = anim->mTicksPerSecond > 0.0 ? static_cast<float>(anim->mTicksPerSecond) : 25.0F;
            clip.setDuration(static_cast<float>(anim->mDuration) / tps);
            clip.setLooping(true);
            nova3d::animation::AnimationTrack track{};
            track.name = clip.name();
            for (unsigned c = 0; c < anim->mNumChannels; ++c) {
                const aiNodeAnim* channel = anim->mChannels[c];
                nova3d::animation::AnimationChannel outChannel{};
                outChannel.boneIndex = -1;
                const auto channelName = std::string(channel->mNodeName.C_Str());
                for (const auto& skeleton : result.data.skeletons) {
                    const auto idx = skeleton.hierarchy().findBone(channelName);
                    if (idx >= 0) { outChannel.boneIndex = idx; break; }
                }
                const auto keys = std::max({channel->mNumPositionKeys, channel->mNumRotationKeys, channel->mNumScalingKeys});
                for (unsigned k = 0; k < keys; ++k) {
                    nova3d::animation::Keyframe key{};
                    const unsigned pk = std::min(k, channel->mNumPositionKeys ? channel->mNumPositionKeys - 1 : 0U);
                    const unsigned rk = std::min(k, channel->mNumRotationKeys ? channel->mNumRotationKeys - 1 : 0U);
                    const unsigned sk = std::min(k, channel->mNumScalingKeys ? channel->mNumScalingKeys - 1 : 0U);
                    key.time = keys > 0 ? static_cast<float>((channel->mNumPositionKeys ? channel->mPositionKeys[pk].mTime : channel->mRotationKeys[rk].mTime) / tps) : 0.0F;
                    if (channel->mNumPositionKeys) { const auto& p = channel->mPositionKeys[pk].mValue; key.transform.translation = {p.x,p.y,p.z}; }
                    if (channel->mNumRotationKeys) { const auto& q = channel->mRotationKeys[rk].mValue; key.transform.rotation = {q.x,q.y,q.z,q.w}; }
                    if (channel->mNumScalingKeys) { const auto& sc = channel->mScalingKeys[sk].mValue; key.transform.scale = {sc.x,sc.y,sc.z}; }
                    outChannel.keyframes.push_back(key);
                }
                if (outChannel.keyframes.empty()) result.report.warnings.emplace_back("Animation channel without keyframes: " + channelName);
                if (outChannel.boneIndex < 0) result.report.warnings.emplace_back("Animation channel not mapped to skeleton bone: " + channelName);
                track.channels.push_back(std::move(outChannel));
            }
            clip.tracks().push_back(std::move(track));
            result.data.animationClips.push_back(std::move(clip));
        }

        if (result.data.materials.empty()) {
            result.data.materials.push_back(NMaterial{"default_engine", "", "", ""});
            result.report.warnings.emplace_back("Model missing materials; default engine material injected.");
        }
        result.report.skeletonCount = static_cast<uint32_t>(result.data.skeletons.size());
        result.report.success = result.report.errors.empty();
        return result;
    }
};

} // namespace

namespace nova::asset {

NModelImporter::NModelImporter() = default;

NModelImportResult NModelImporter::importFromFile(const std::string& path, const NImportOptions& options) const {
    const std::string ext = normalizeExtension(path);
    static const std::unordered_set<std::string> internalFormats = {".obj"};

    bool useAssimpFallback = (internalFormats.find(ext) == internalFormats.end());
    NAssimpImporterBackend backend;
    return backend.importModel(path, options, useAssimpFallback, ext);
}

} // namespace nova::asset
