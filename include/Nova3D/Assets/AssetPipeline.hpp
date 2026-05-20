#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>
#include <Nova3D/Serialization/Serialization.hpp>

namespace nova3d::assets {

struct AssetGUID { std::string value; static AssetGUID fromPath(std::string_view path); bool operator==(const AssetGUID&) const = default; };
struct AssetMetadata { AssetGUID guid; std::string sourcePath; std::string typeName; std::vector<AssetGUID> dependencies; std::unordered_map<std::string,std::string> tags; };
struct AssetManifest { std::vector<AssetMetadata> entries; };

class AssetDatabase {
public:
    void registerAsset(AssetMetadata metadata);
    const AssetMetadata* findByGuid(const AssetGUID& guid) const;
    const AssetMetadata* findByPath(std::string_view path) const;
    std::vector<const AssetMetadata*> findByType(std::string_view typeName) const;
    const AssetManifest& manifest() const { return m_manifest; }
private:
    AssetManifest m_manifest;
    std::unordered_map<std::string,size_t> m_guidToIndex;
    std::unordered_map<std::string,size_t> m_pathToIndex;
};

class AssetResolver {
public:
    explicit AssetResolver(const AssetDatabase& db): m_db(db) {}
    std::vector<const AssetMetadata*> resolveDependencies(const AssetGUID& guid) const;
private:
    const AssetDatabase& m_db;
};

std::string serializeAssetMetadata(const AssetMetadata& metadata);
std::optional<AssetMetadata> deserializeAssetMetadata(const std::string& json);

}
