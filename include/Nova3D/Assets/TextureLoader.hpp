#pragma once
#include <Nova3D/Graphics/RendererTypes.hpp>
#include <Nova3D/IO/FileSystem.hpp>
#include <string>

namespace nova3d::assets {
class TextureLoader {
public:
    static bool loadImage(io::IFileSystem& fs, const std::string& path, graphics::ImageData& outImage, std::string& outError);
};
}
