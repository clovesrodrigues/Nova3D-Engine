#include <Nova3D/Assets/Importers/NModelImporter.hpp>

#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: nova_asset_validator <model-path>\n";
        return 2;
    }

    nova::asset::NModelImporter importer;
    const auto result = importer.importFromFile(argv[1]);
    const auto& report = result.report;

    std::cout << "sourcePath: " << report.sourcePath << "\n";
    std::cout << "detectedExtension: " << report.detectedExtension << "\n";
    std::cout << "usedAssimpFallback: " << (report.usedAssimpFallback ? "true" : "false") << "\n";
    std::cout << "nodeCount: " << report.nodeCount << "\n";
    std::cout << "meshCount: " << report.meshCount << "\n";
    std::cout << "materialCount: " << report.materialCount << "\n";
    std::cout << "animationCount: " << report.animationCount << "\n";
    std::cout << "skeletonCount: " << report.skeletonCount << "\n";

    std::cout << "warnings(" << report.warnings.size() << "):\n";
    for (const auto& w : report.warnings) std::cout << "  - " << w << "\n";
    std::cout << "errors(" << report.errors.size() << "):\n";
    for (const auto& e : report.errors) std::cout << "  - " << e << "\n";

    std::cout << "success: " << (report.success ? "true" : "false") << "\n";
    return report.success ? 0 : 1;
}
