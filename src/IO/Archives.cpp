#include <Nova3D/IO/Archives.hpp>
#include <filesystem>
#include <fstream>

namespace nova3d::io {
bool ZipArchive::contains(const std::string& path) const { return std::filesystem::exists(m_root + "/" + path); }
std::vector<std::uint8_t> ZipArchive::readBinary(const std::string& path) const { std::ifstream f(m_root + "/" + path, std::ios::binary); return {std::istreambuf_iterator<char>(f), {}}; }
std::shared_ptr<IFileArchive> ArchiveFactory::create(const std::string& mountPath) { if (mountPath.ends_with(".zip") || std::filesystem::is_directory(mountPath)) return std::make_shared<ZipArchive>(mountPath); if (mountPath.ends_with(".pak") || mountPath.ends_with(".pk3")) return std::make_shared<PakArchive>(); return {}; }
}
