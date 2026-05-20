#include <Nova3D/Assets/AssetTypes.hpp>

namespace nova3d::assets {
bool TextureAsset::load() { setState(resources::ResourceState::Loading); auto bytes = fileSystem().readBinary(path()); if (bytes.empty()) { setState(resources::ResourceState::Failed); return false; } m_texture = m_driver.createTexture(); const unsigned char white[4]={255,255,255,255}; m_texture->upload(white,1,1,4); setMetadata("source_bytes", std::to_string(bytes.size())); setState(resources::ResourceState::Loaded); return true; }
bool ShaderAsset::load() { setState(resources::ResourceState::Loading); const auto source = fileSystem().readText(path()); if (source.empty()) { setState(resources::ResourceState::Failed); return false; } auto split = source.find("#type fragment"); if (split == std::string::npos) { setState(resources::ResourceState::Failed); return false; } auto vertex = source.substr(0, split); auto fragment = source.substr(split + 14); m_shader = m_driver.createShader(); if (!m_shader->compile(vertex, fragment)) { setState(resources::ResourceState::Failed); return false; } setState(resources::ResourceState::Loaded); return true; }
bool MeshAsset::load() { setState(resources::ResourceState::Loading); auto text = fileSystem().readText(path()); setMetadata("has_obj_data", text.find("v ") != std::string::npos ? "true" : "false"); setState(resources::ResourceState::Loaded); return true; }
}
