#include <Nova3D/Resources/HotReloadHooks.hpp>
namespace nova3d::resources { void HotReloadHooks::invalidate(const std::string& id){ for(auto& cb:m_invalidate) cb(id);} }
