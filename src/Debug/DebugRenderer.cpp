#include <Nova3D/Debug/DebugRenderer.hpp>
namespace nova3d::debug {
void DebugRenderer::drawLine(const math::Vector3& a, const math::Vector3& b, const math::Vector4& color, float lifetime){ m_lines.push_back({a,b,color,lifetime}); }
void DebugRenderer::drawBox(const math::AABB& box, const math::Vector4& color, float lifetime){ drawLine(box.min,{box.max.x,box.min.y,box.min.z},color,lifetime); drawLine(box.min,{box.min.x,box.max.y,box.min.z},color,lifetime); drawLine(box.min,{box.min.x,box.min.y,box.max.z},color,lifetime); }
void DebugRenderer::drawSphere(const math::Vector3& center, float radius, const math::Vector4& color, float lifetime){ drawLine({center.x-radius,center.y,center.z},{center.x+radius,center.y,center.z},color,lifetime); }
void DebugRenderer::drawFrustum(const math::Frustum& frustum, const math::Vector4& color, float lifetime){ for(const auto& p:frustum.planes) drawLine(p.normal,p.normal*2.0F,color,lifetime); }
void DebugRenderer::drawOctreeBounds(const math::AABB& bounds, const math::Vector4& color, float lifetime){ drawBox(bounds,color,lifetime); }
void DebugRenderer::newFrame(float dt){ for(auto& l:m_lines) l.lifetime-=dt; m_lines.erase(std::remove_if(m_lines.begin(),m_lines.end(),[](const DebugLine& l){ return l.lifetime<0.0F; }),m_lines.end()); }
}
