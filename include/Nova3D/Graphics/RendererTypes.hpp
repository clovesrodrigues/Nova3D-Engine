#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace nova3d::graphics {

enum class GraphicsBackend { OpenGL, OpenGLES, Vulkan };
enum class TextureFilter { Nearest, Linear };
enum class TextureWrap { Repeat, ClampToEdge };
enum class TextureFormat { RGB8, RGBA8 };
enum class PrimitiveType { Triangles };
enum class VertexSemantic { Position, Normal, UV0, Color0, Tangent };

struct VertexAttribute {
    VertexSemantic semantic = VertexSemantic::Position;
    std::uint32_t location = 0;
    std::uint32_t components = 3;
    std::size_t offset = 0;
};

struct VertexLayout {
    std::size_t stride = 0;
    VertexAttribute attributes[8]{};
    std::size_t attributeCount = 0;
};

struct Viewport { int x=0; int y=0; int width=0; int height=0; };
struct Color { float r=0.0F; float g=0.0F; float b=0.0F; float a=1.0F; };
struct RenderState { bool depthTest=true; bool depthWrite=true; bool culling=false; bool wireframe=false; bool blending=false; };

struct ImageData {
    int width = 0;
    int height = 0;
    int channels = 0;
    TextureFormat format = TextureFormat::RGBA8;
    std::vector<std::uint8_t> pixels;
};

} // namespace
