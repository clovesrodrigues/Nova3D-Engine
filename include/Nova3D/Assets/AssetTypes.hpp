#pragma once
#include <Nova3D/Graphics/IRenderer.hpp>
#include <Nova3D/IO/FileSystem.hpp>
#include <Nova3D/Resources/Resource.hpp>
#include <unordered_map>

namespace nova3d::assets {

class AssetBase : public resources::IResource {
public:
    AssetBase(std::string id, std::string path, io::IFileSystem& fs) : IResource(std::move(id), std::move(path)), m_fs(fs) {}
protected:
    io::IFileSystem& fileSystem() const { return m_fs; }
private:
    io::IFileSystem& m_fs;
};

class TextureAsset final : public AssetBase { public: TextureAsset(std::string p, io::IFileSystem& fs, graphics::IVideoDriver& d) : AssetBase(p,p,fs), m_driver(d) {} bool load() override; void unload() override { m_texture.reset(); setState(resources::ResourceState::Unloaded);} std::shared_ptr<graphics::ITexture> texture() const { return m_texture; } bool ensureUploaded(); void invalidateGpu(); private: graphics::IVideoDriver& m_driver; graphics::ImageData m_image; std::shared_ptr<graphics::ITexture> m_texture; bool m_gpuUploaded=false;};
class ShaderAsset final : public AssetBase { public: ShaderAsset(std::string p, io::IFileSystem& fs, graphics::IVideoDriver& d) : AssetBase(p,p,fs), m_driver(d) {} bool load() override; void unload() override { m_shader.reset(); setState(resources::ResourceState::Unloaded);} std::shared_ptr<graphics::IShader> shader() const { return m_shader; } private: graphics::IVideoDriver& m_driver; std::shared_ptr<graphics::IShader> m_shader;};
class MeshAsset final : public AssetBase { public: MeshAsset(std::string p, io::IFileSystem& fs) : AssetBase(p,p,fs) {} bool load() override; void unload() override { setState(resources::ResourceState::Unloaded);} };
class MaterialAsset final : public AssetBase { public: MaterialAsset(std::string p, io::IFileSystem& fs) : AssetBase(p,p,fs) {} bool load() override { setState(resources::ResourceState::Loaded); return true; } void unload() override { setState(resources::ResourceState::Unloaded);} void setShaderPath(std::string p){m_shaderPath=std::move(p);} void setTextureSlot(std::string slot,std::string texture){m_textureSlots[std::move(slot)]=std::move(texture);} graphics::RenderState& renderState(){return m_state;} const auto& textureSlots() const { return m_textureSlots; } private: std::string m_shaderPath; std::unordered_map<std::string,std::string> m_textureSlots; graphics::RenderState m_state{};};

} // namespace
