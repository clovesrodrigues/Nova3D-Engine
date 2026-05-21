#include <Nova3D/Assets/Importers/NModelImporter.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

struct Expectation {
    std::string format;
    std::string modelPath;
    bool expectSuccess = true;
    int minMeshes = 0;
    int minMaterials = 0;
    bool expectAnimation = false;
    bool expectSkeleton = false;
    bool expectFallbackWarnings = false;
};

static std::string readText(const fs::path& p) {
    std::ifstream in(p);
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static bool extractString(const std::string& s, const std::string& key, std::string& out) {
    std::regex re("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
    std::smatch m;
    if (std::regex_search(s, m, re)) { out = m[1].str(); return true; }
    return false;
}
static bool extractInt(const std::string& s, const std::string& key, int& out) {
    std::regex re("\"" + key + "\"\\s*:\\s*(-?[0-9]+)");
    std::smatch m;
    if (std::regex_search(s, m, re)) { out = std::stoi(m[1].str()); return true; }
    return false;
}
static bool extractBool(const std::string& s, const std::string& key, bool& out) {
    std::regex re("\"" + key + "\"\\s*:\\s*(true|false)");
    std::smatch m;
    if (std::regex_search(s, m, re)) { out = (m[1].str() == "true"); return true; }
    return false;
}

static bool loadExpectation(const fs::path& metaPath, Expectation& e) {
    const auto text = readText(metaPath);
    return extractString(text, "format", e.format)
        && extractString(text, "model", e.modelPath)
        && extractBool(text, "expect_success", e.expectSuccess)
        && extractInt(text, "min_meshes", e.minMeshes)
        && extractInt(text, "min_materials", e.minMaterials)
        && extractBool(text, "expect_animation", e.expectAnimation)
        && extractBool(text, "expect_skeleton", e.expectSkeleton)
        && extractBool(text, "expect_fallback_warnings", e.expectFallbackWarnings);
}

int main(int argc, char** argv) {
    const fs::path root = argc > 1 ? fs::path(argv[1]) : fs::path("assets/tests/import");
    if (!fs::exists(root)) {
        std::cerr << "Missing asset test root: " << root << "\n";
        return 2;
    }

    int passed = 0;
    int failed = 0;
    nova::asset::NModelImporter importer;

    for (auto const& it : fs::recursive_directory_iterator(root)) {
        if (!it.is_regular_file() || it.path().extension() != ".json") continue;
        Expectation e;
        if (!loadExpectation(it.path(), e)) {
            std::cerr << "Invalid metadata: " << it.path() << "\n";
            failed++;
            continue;
        }
        fs::path model = it.path().parent_path() / e.modelPath;
        auto result = importer.importFromFile(model.string());
        auto& r = result.report;

        bool ok = true;
        ok = ok && (r.success == e.expectSuccess);
        ok = ok && (static_cast<int>(r.meshCount) >= e.minMeshes);
        ok = ok && (static_cast<int>(r.materialCount) >= e.minMaterials);
        ok = ok && ((r.animationCount > 0) == e.expectAnimation);
        ok = ok && ((r.skeletonCount > 0) == e.expectSkeleton);
        if (e.expectFallbackWarnings) ok = ok && !r.warnings.empty();

        std::cout << "[" << (ok ? "PASS" : "FAIL") << "] " << e.format << " :: " << model.string() << "\n";
        if (ok) passed++; else {
            failed++;
            std::cout << "  success=" << r.success << " meshes=" << r.meshCount << " materials=" << r.materialCount
                      << " anims=" << r.animationCount << " skeletons=" << r.skeletonCount << " warnings=" << r.warnings.size() << "\n";
            for (const auto& err : r.errors) std::cout << "  error: " << err << "\n";
        }
    }

    std::cout << "Summary: passed=" << passed << " failed=" << failed << "\n";
    return failed == 0 ? 0 : 1;
}
