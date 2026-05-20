#pragma once
#include <memory>
#include <Nova3D/IO/FileSystem.hpp>

namespace nova3d::io {
class ZipArchive final : public IFileArchive { public: explicit ZipArchive(std::string root) : m_root(std::move(root)) {} bool contains(const std::string& path) const override; std::vector<std::uint8_t> readBinary(const std::string& path) const override; private: std::string m_root; };
class PakArchive final : public IFileArchive { public: bool contains(const std::string&) const override { return false; } std::vector<std::uint8_t> readBinary(const std::string&) const override { return {}; } };
class ArchiveFactory { public: static std::shared_ptr<IFileArchive> create(const std::string& mountPath); };
}
