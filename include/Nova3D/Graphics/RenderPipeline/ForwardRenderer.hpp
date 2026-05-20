#pragma once
#include <memory>
#include <vector>
#include <Nova3D/Graphics/IRenderer.hpp>
#include <Nova3D/Scene/Scene.hpp>

namespace nova3d::graphics {
struct CameraRenderContext { math::Matrix4 view; math::Matrix4 proj; math::Matrix4 viewProj; Viewport viewport; std::shared_ptr<IRenderTarget> target; };
class RenderPass { public: virtual ~RenderPass()=default; virtual void execute(IVideoDriver&, scene::SceneManager&, const CameraRenderContext&)=0; };
class ForwardRenderer { public: explicit ForwardRenderer(IVideoDriver& driver); void render(scene::SceneManager& scene); private: IVideoDriver& m_driver; std::vector<std::unique_ptr<RenderPass>> m_passes; };
}
