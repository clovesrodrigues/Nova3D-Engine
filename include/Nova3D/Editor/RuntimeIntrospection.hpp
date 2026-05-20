#pragma once
#include <Nova3D/Core/ObjectRegistry.hpp>
#include <Nova3D/Reflection/Reflection.hpp>
#include <string>
#include <vector>

namespace nova3d::editor {
struct InspectionEntry { core::ObjectId id{}; std::string type; std::vector<reflection::PropertyInfo> properties; };
class RuntimeIntrospection { public: static InspectionEntry inspect(core::ObjectId id, const std::string& type); };
}
