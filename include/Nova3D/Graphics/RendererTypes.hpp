#pragma once

#include <cstdint>

namespace nova3d::graphics {

enum class GraphicsBackend {
    OpenGL,
    OpenGLES,
    Vulkan
};

enum class TextureFilter {
    Nearest,
    Linear
};

enum class TextureWrap {
    Repeat,
    ClampToEdge
};

enum class PrimitiveType {
    Triangles
};

struct Viewport {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

struct Color {
    float r = 0.0F;
    float g = 0.0F;
    float b = 0.0F;
    float a = 1.0F;
};

struct RenderState {
    bool depthTest = true;
    bool depthWrite = true;
    bool culling = false;
    bool wireframe = false;
    bool blending = false;
};

} // namespace nova3d::graphics
