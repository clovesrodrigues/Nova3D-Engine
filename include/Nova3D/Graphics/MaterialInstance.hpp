#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <Nova3D/Graphics/IRenderer.hpp>
namespace nova3d::graphics {
using MaterialParamValue = std::variant<float,int,bool,math::Vector2,math::Vector3,math::Vector4,math::Matrix4>;
class MaterialTemplate { public: std::string name; RenderState baseState{}; std::shared_ptr<IShader> shader; std::unordered_map<std::uint32_t,std::shared_ptr<ITexture>> textures; std::unordered_map<std::string,MaterialParamValue> defaults; };
class MaterialInstance { public: explicit MaterialInstance(std::shared_ptr<MaterialTemplate> t):m_template(std::move(t)){} void setTextureOverride(std::uint32_t slot,std::shared_ptr<ITexture> tex){m_textures[slot]=std::move(tex);} void setParam(const std::string& k,const MaterialParamValue& v){m_params[k]=v;} bool tryGetParam(const std::string& k,MaterialParamValue& out) const; void setStateOverride(const RenderState& s){m_state=s;m_hasState=true;} RenderState resolvedState() const; void bind(IMaterial& material) const; private: std::shared_ptr<MaterialTemplate> m_template; std::unordered_map<std::uint32_t,std::shared_ptr<ITexture>> m_textures; std::unordered_map<std::string,MaterialParamValue> m_params; bool m_hasState=false; RenderState m_state{}; };
}
