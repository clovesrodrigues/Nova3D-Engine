#include <Nova3D/Reflection/Reflection.hpp>

namespace nova3d::reflection {
void ObjectFactory::registerType(const std::string& n, ConstructFn fn){ m_types[n]=std::move(fn);} 
std::shared_ptr<void> ObjectFactory::create(std::string_view name) const { if(auto it=m_types.find(std::string(name)); it!=m_types.end()) return it->second(); return {}; }
TypeRegistry& TypeRegistry::instance(){ static TypeRegistry r; return r; }
void TypeRegistry::registerType(TypeInfo info){ m_types[info.name]=std::move(info);} 
const TypeInfo* TypeRegistry::find(std::string_view name) const { if(auto it=m_types.find(std::string(name)); it!=m_types.end()) return &it->second; return nullptr; }
}
