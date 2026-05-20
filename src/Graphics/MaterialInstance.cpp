#include <Nova3D/Graphics/MaterialInstance.hpp>
namespace nova3d::graphics {
bool MaterialInstance::tryGetParam(const std::string& k,MaterialParamValue& out) const { auto it=m_params.find(k); if(it!=m_params.end()){out=it->second; return true;} if(!m_template) return false; auto dt=m_template->defaults.find(k); if(dt==m_template->defaults.end()) return false; out=dt->second; return true; }
RenderState MaterialInstance::resolvedState() const { return m_hasState?m_state:(m_template?m_template->baseState:RenderState{}); }
void MaterialInstance::bind(IMaterial& material) const { if(m_template){ material.setShader(m_template->shader); for(auto& [slot,tex]:m_template->textures) material.setTexture(slot,tex);} for(auto& [slot,tex]:m_textures) material.setTexture(slot,tex); material.setState(resolvedState()); }
}
