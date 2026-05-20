#include <Nova3D/Core/ObjectRegistry.hpp>
namespace nova3d::core {
std::shared_ptr<void> ObjectRegistry::resolve(ObjectId id) const { if(auto it=m_objects.find(id); it!=m_objects.end()) return it->second.lock(); return {}; }
void ObjectRegistry::unregisterObject(ObjectId id){ m_objects.erase(id);} 
}
