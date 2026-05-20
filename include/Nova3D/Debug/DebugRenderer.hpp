#pragma once
#include <Nova3D/Core/Math/Math.hpp>
#include <vector>

namespace nova3d::debug {
struct DebugLine { math::Vector3 a; math::Vector3 b; math::Vector4 color; float lifetime=0.0F; };
class DebugRenderer {
public:
    void drawLine(const math::Vector3& a, const math::Vector3& b, const math::Vector4& color, float lifetime=0.0F);
    void drawBox(const math::AABB& box, const math::Vector4& color, float lifetime=0.0F);
    void drawSphere(const math::Vector3& center, float radius, const math::Vector4& color, float lifetime=0.0F);
    void drawFrustum(const math::Frustum& frustum, const math::Vector4& color, float lifetime=0.0F);
    void drawOctreeBounds(const math::AABB& bounds, const math::Vector4& color, float lifetime=0.0F);
    void newFrame(float dt);
    const std::vector<DebugLine>& lines() const { return m_lines; }
private:
    std::vector<DebugLine> m_lines;
};
}
