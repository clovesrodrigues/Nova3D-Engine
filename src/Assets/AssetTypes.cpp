#include <Nova3D/Assets/TextureLoader.hpp>
#include <Nova3D/Assets/AssetTypes.hpp>

namespace nova3d::assets {
bool TextureAsset::load() { setState(resources::ResourceState::Loading); std::string error; if (!TextureLoader::loadImage(fileSystem(), path(), m_image, error)) { setMetadata("error", error); setState(resources::ResourceState::Failed); return false; } setMetadata("width", std::to_string(m_image.width)); setMetadata("height", std::to_string(m_image.height)); setState(resources::ResourceState::Loaded); return true; }

bool TextureAsset::ensureUploaded() { if (m_gpuUploaded) return true; if (m_image.pixels.empty()) return false; if (!m_texture) m_texture = m_driver.createTexture(); m_gpuUploaded = m_texture->upload(m_image); return m_gpuUploaded; }

void TextureAsset::invalidateGpu() { m_gpuUploaded = false; m_texture.reset(); }
bool ShaderAsset::load() { setState(resources::ResourceState::Loading); const auto source = fileSystem().readText(path()); if (source.empty()) { setState(resources::ResourceState::Failed); return false; } auto split = source.find("#type fragment"); if (split == std::string::npos) { setState(resources::ResourceState::Failed); return false; } auto vertex = source.substr(0, split); auto fragment = source.substr(split + 14); m_shader = m_driver.createShader(); if (!m_shader->compile(vertex, fragment)) { setState(resources::ResourceState::Failed); return false; } setState(resources::ResourceState::Loaded); return true; }
bool MeshAsset::load() { setState(resources::ResourceState::Loading); auto text = fileSystem().readText(path()); setMetadata("has_obj_data", text.find("v ") != std::string::npos ? "true" : "false"); setState(resources::ResourceState::Loaded); return true; }
}
