#pragma once
#include <functional>
#include <string>
#include <unordered_map>

namespace nova3d::assets {

class AssetRegistry {
public:
    using ImporterFactory = std::function<void()>;
    void registerImporter(const std::string& extension, ImporterFactory factory);
    bool hasImporter(const std::string& extension) const;

private:
    std::unordered_map<std::string, ImporterFactory> importers_;
};

} // namespace nova3d::assets
