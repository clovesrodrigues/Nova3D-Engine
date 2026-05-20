#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nova3d::io {

class IFileArchive {
public:
    virtual ~IFileArchive() = default;
    virtual bool contains(const std::string& path) const = 0;
    virtual std::vector<std::uint8_t> readBinary(const std::string& path) const = 0;
};

class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    virtual bool exists(const std::string& path) const = 0;
    virtual std::vector<std::uint8_t> readBinary(const std::string& path) const = 0;
    virtual std::string readText(const std::string& path) const = 0;
    virtual void mount(const std::shared_ptr<IFileArchive>& archive) = 0;
};

class FileSystem final : public IFileSystem {
public:
    bool exists(const std::string& path) const override;
    std::vector<std::uint8_t> readBinary(const std::string& path) const override;
    std::string readText(const std::string& path) const override;
    void mount(const std::shared_ptr<IFileArchive>& archive) override;
private:
    std::vector<std::shared_ptr<IFileArchive>> m_archives;
};

} // namespace
