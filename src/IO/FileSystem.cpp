#include <Nova3D/IO/FileSystem.hpp>
#include <filesystem>
#include <fstream>

namespace nova3d::io {
bool FileSystem::exists(const std::string& path) const { if (std::filesystem::exists(path)) return true; for (const auto& a : m_archives) if (a->contains(path)) return true; return false; }
std::vector<std::uint8_t> FileSystem::readBinary(const std::string& path) const { if (std::filesystem::exists(path)) { std::ifstream f(path, std::ios::binary); return {std::istreambuf_iterator<char>(f), {}}; } for (const auto& a : m_archives) if (a->contains(path)) return a->readBinary(path); return {}; }
std::string FileSystem::readText(const std::string& path) const { auto b = readBinary(path); return {reinterpret_cast<const char*>(b.data()), b.size()}; }
void FileSystem::mount(const std::shared_ptr<IFileArchive>& archive) { m_archives.push_back(archive); }
}
