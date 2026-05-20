#include <Nova3D/Assets/TextureLoader.hpp>
#include <cstring>

namespace nova3d::assets {
static bool loadBmp(const std::vector<std::uint8_t>& b, graphics::ImageData& out) {
    if (b.size() < 54 || b[0] != 'B' || b[1] != 'M') return false;
    const int dataOffset = *reinterpret_cast<const int*>(&b[10]);
    const int width = *reinterpret_cast<const int*>(&b[18]);
    const int height = *reinterpret_cast<const int*>(&b[22]);
    const int bpp = *reinterpret_cast<const short*>(&b[28]);
    if (bpp != 24 && bpp != 32) return false;
    const int ch = bpp / 8;
    const int rowStride = ((width * ch + 3) / 4) * 4;
    out.width = width; out.height = height; out.channels = 4; out.format = graphics::TextureFormat::RGBA8;
    out.pixels.assign(width * height * 4, 255);
    for (int y=0;y<height;++y) {
        const auto* src = b.data() + dataOffset + (height - 1 - y) * rowStride;
        for (int x=0;x<width;++x) {
            const int si = x * ch; const int di = (y * width + x) * 4;
            out.pixels[di+0] = src[si+2]; out.pixels[di+1] = src[si+1]; out.pixels[di+2] = src[si+0];
            if (ch == 4) out.pixels[di+3] = src[si+3];
        }
    }
    return true;
}

bool TextureLoader::loadImage(io::IFileSystem& fs, const std::string& path, graphics::ImageData& outImage, std::string& outError) {
    const auto bytes = fs.readBinary(path);
    if (bytes.empty()) { outError = "Image file empty"; return false; }
    if (loadBmp(bytes, outImage)) return true;
    outError = "Unsupported image format (currently BMP runtime loader in sandbox env)";
    return false;
}
}
