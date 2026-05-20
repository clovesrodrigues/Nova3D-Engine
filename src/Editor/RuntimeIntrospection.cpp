#include <Nova3D/Editor/RuntimeIntrospection.hpp>
namespace nova3d::editor {
InspectionEntry RuntimeIntrospection::inspect(core::ObjectId id, const std::string& type){ InspectionEntry e; e.id=id; e.type=type; if(auto* ti=reflection::TypeRegistry::instance().find(type)) e.properties=ti->properties; return e; }
}
