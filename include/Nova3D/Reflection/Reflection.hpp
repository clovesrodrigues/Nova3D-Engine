#pragma once
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nova3d::reflection {

struct PropertyMetadata { bool editable=true; float minValue=0.0F; float maxValue=0.0F; std::string category{"General"}; bool visible=true; bool serializable=true; };
struct PropertyInfo { std::string name; std::string typeName; PropertyMetadata metadata; };
struct MethodInfo { std::string name; std::string signature; };
struct EnumInfo { std::string name; std::vector<std::string> values; };
struct TypeInfo { std::string name; std::string baseType; std::vector<PropertyInfo> properties; std::vector<MethodInfo> methods; };

class ObjectFactory { public: using ConstructFn=std::function<std::shared_ptr<void>()>; void registerType(const std::string& n, ConstructFn fn); std::shared_ptr<void> create(std::string_view name) const; private: std::unordered_map<std::string, ConstructFn> m_types;};

class TypeRegistry { public: static TypeRegistry& instance(); void registerType(TypeInfo info); const TypeInfo* find(std::string_view name) const; const std::unordered_map<std::string, TypeInfo>& all() const { return m_types; } private: std::unordered_map<std::string, TypeInfo> m_types;};

} // namespace
