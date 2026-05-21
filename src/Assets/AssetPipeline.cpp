#include <Nova3D/Assets/AssetPipeline.hpp>
#include <Nova3D/Serialization/JsonSerialization.hpp>
#include <functional>

namespace nova3d::assets {
AssetGUID AssetGUID::fromPath(std::string_view path) { return {std::to_string(std::hash<std::string_view>{}(path))}; }
void AssetDatabase::registerAsset(AssetMetadata metadata){ if(metadata.guid.value.empty()) metadata.guid=AssetGUID::fromPath(metadata.sourcePath); auto it=m_guidToIndex.find(metadata.guid.value); if(it!=m_guidToIndex.end()){m_manifest.entries[it->second]=std::move(metadata);return;} const auto idx=m_manifest.entries.size(); m_pathToIndex[metadata.sourcePath]=idx; m_guidToIndex[metadata.guid.value]=idx; m_manifest.entries.push_back(std::move(metadata)); }
const AssetMetadata* AssetDatabase::findByGuid(const AssetGUID& guid) const { auto it=m_guidToIndex.find(guid.value); return it==m_guidToIndex.end()?nullptr:&m_manifest.entries[it->second]; }
const AssetMetadata* AssetDatabase::findByPath(std::string_view path) const { auto it=m_pathToIndex.find(std::string(path)); return it==m_pathToIndex.end()?nullptr:&m_manifest.entries[it->second]; }
std::vector<const AssetMetadata*> AssetDatabase::findByType(std::string_view typeName) const { std::vector<const AssetMetadata*> out; for (const auto& e: m_manifest.entries) if (e.typeName==typeName) out.push_back(&e); return out; }
std::vector<const AssetMetadata*> AssetResolver::resolveDependencies(const AssetGUID& guid) const { std::vector<const AssetMetadata*> out; if (const auto* m = m_db.findByGuid(guid)) for (const auto& dep:m->dependencies) if (const auto* d=m_db.findByGuid(dep)) out.push_back(d); return out; }
std::string serializeAssetMetadata(const AssetMetadata& metadata){ serialization::JsonSerializer s; auto& root=s.root(); root["guid"].value=metadata.guid.value; root["sourcePath"].value=metadata.sourcePath; root["typeName"].value=metadata.typeName; serialization::SerializedValue::Array deps; for(const auto& d:metadata.dependencies){ serialization::SerializedValue v; v.value=d.value; deps.push_back(std::move(v)); } root["dependencies"].value=std::move(deps); serialization::SerializedValue::Object tags; for(const auto& [k,v]:metadata.tags){ serialization::SerializedValue t; t.value=v; tags[k]=std::move(t);} root["tags"].value=std::move(tags); return s.toJsonString(); }
std::optional<AssetMetadata> deserializeAssetMetadata(const std::string&){ return std::nullopt; }
}
