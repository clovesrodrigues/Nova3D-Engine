#include <Nova3D/Assets/Importers/NModelImporter.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct CaseExpectation {
    std::string caseName;
    std::string format;
    std::string assetPath;
    bool expectedSuccess = true;
    std::optional<int> expectedNodeCountExact;
    std::optional<int> expectedNodeCountMin;
    std::optional<int> expectedMeshCountExact;
    std::optional<int> expectedMeshCountMin;
    std::optional<int> expectedMaterialCountExact;
    std::optional<int> expectedMaterialCountMin;
    std::optional<int> expectedAnimationCountExact;
    std::optional<int> expectedAnimationCountMin;
    bool expectSkeleton = false;
    std::optional<int> expectedChannelCountMin;
    std::optional<int> expectedBoneCountMin;
    std::optional<int> expectedKeyframesPerChannelMin;
    bool expectDurationPositive = false;
    std::vector<std::string> expectedWarningsContains;
    std::vector<std::string> expectedFallbackFlags;
};

static std::string readText(const fs::path& p) { std::ifstream in(p); std::stringstream ss; ss << in.rdbuf(); return ss.str(); }
static std::optional<std::string> extractString(const std::string& s, const std::string& key) { std::regex re("\""+key+"\"\\s*:\\s*\"([^\"]*)\""); std::smatch m; if(std::regex_search(s,m,re)) return m[1].str(); return std::nullopt; }
static std::optional<int> extractInt(const std::string& s, const std::string& key) { std::regex re("\""+key+"\"\\s*:\\s*(-?[0-9]+)"); std::smatch m; if(std::regex_search(s,m,re)) return std::stoi(m[1].str()); return std::nullopt; }
static std::optional<bool> extractBool(const std::string& s, const std::string& key) { std::regex re("\""+key+"\"\\s*:\\s*(true|false)"); std::smatch m; if(std::regex_search(s,m,re)) return m[1].str()=="true"; return std::nullopt; }
static std::vector<std::string> extractStringArray(const std::string& s, const std::string& key){ std::vector<std::string> out; std::regex re("\""+key+"\"\\s*:\\s*\\[([^\\]]*)\\]"); std::smatch m; if(!std::regex_search(s,m,re)) return out; std::regex item("\"([^\"]+)\""); auto body=m[1].str(); for(std::sregex_iterator i(body.begin(), body.end(), item),e; i!=e; ++i){ out.push_back((*i)[1].str()); } return out; }

static bool loadCase(const fs::path& manifest, CaseExpectation& c) {
    const auto t = readText(manifest);
    c.caseName = extractString(t, "caseName").value_or(manifest.stem().string());
    c.format = extractString(t, "format").value_or("unknown");
    auto asset = extractString(t, "asset");
    auto success = extractBool(t, "expectedSuccess");
    if (!asset.has_value() || !success.has_value()) return false;
    c.assetPath = *asset;
    c.expectedSuccess = *success;
    c.expectedNodeCountExact = extractInt(t, "expectedNodeCountExact");
    c.expectedNodeCountMin = extractInt(t, "expectedNodeCountMin");
    c.expectedMeshCountExact = extractInt(t, "expectedMeshCountExact");
    c.expectedMeshCountMin = extractInt(t, "expectedMeshCountMin");
    c.expectedMaterialCountExact = extractInt(t, "expectedMaterialCountExact");
    c.expectedMaterialCountMin = extractInt(t, "expectedMaterialCountMin");
    c.expectedAnimationCountExact = extractInt(t, "expectedAnimationCountExact");
    c.expectedAnimationCountMin = extractInt(t, "expectedAnimationCountMin");
    c.expectSkeleton = extractBool(t, "expectSkeleton").value_or(false);
    c.expectedChannelCountMin = extractInt(t, "expectedChannelCountMin");
    c.expectedBoneCountMin = extractInt(t, "expectedBoneCountMin");
    c.expectedKeyframesPerChannelMin = extractInt(t, "expectedKeyframesPerChannelMin");
    c.expectDurationPositive = extractBool(t, "expectDurationPositive").value_or(false);
    c.expectedWarningsContains = extractStringArray(t, "expectedWarningsContains");
    c.expectedFallbackFlags = extractStringArray(t, "expectedFallbackFlags");
    return true;
}

static bool hasWarning(const std::vector<std::string>& warnings, const std::string& pattern){ for(const auto& w:warnings){ if(w.find(pattern)!=std::string::npos) return true; } return false; }

int main(int argc, char** argv) {
    const fs::path root = argc > 1 ? fs::path(argv[1]) : fs::path("assets/tests/import");
    const std::string group = argc > 2 ? argv[2] : "all";
    const fs::path expectedDir = root / "expected";
    if (!fs::exists(expectedDir)) { std::cerr << "Missing expected dir: " << expectedDir << "\n"; return 2; }

    int passed = 0, failed = 0;
    nova::asset::NModelImporter importer;
    for (const auto& it : fs::directory_iterator(expectedDir)) {
        if (!it.is_regular_file() || it.path().extension() != ".json") continue;
        CaseExpectation e;
        if (!loadCase(it.path(), e)) { std::cerr << "Invalid manifest: " << it.path() << "\n"; ++failed; continue; }
        if (group != "all" && e.format != group && e.caseName.find(group) == std::string::npos) continue;

        auto result = importer.importFromFile((root / e.assetPath).string());
        auto& r = result.report;
        std::vector<std::string> diffs;
        auto chk = [&](bool cond, const std::string& d){ if(!cond) diffs.push_back(d); };
        chk(r.success == e.expectedSuccess, "success esperado=" + std::string(e.expectedSuccess?"true":"false") + ", atual=" + (r.success?"true":"false"));
        if (e.expectedNodeCountExact) chk(static_cast<int>(r.nodeCount) == *e.expectedNodeCountExact, "nodeCount esperado==" + std::to_string(*e.expectedNodeCountExact) + ", atual=" + std::to_string(r.nodeCount));
        if (e.expectedNodeCountMin) chk(static_cast<int>(r.nodeCount) >= *e.expectedNodeCountMin, "nodeCount esperado>=" + std::to_string(*e.expectedNodeCountMin) + ", atual=" + std::to_string(r.nodeCount));
        if (e.expectedMeshCountExact) chk(static_cast<int>(r.meshCount) == *e.expectedMeshCountExact, "meshCount esperado==" + std::to_string(*e.expectedMeshCountExact) + ", atual=" + std::to_string(r.meshCount));
        if (e.expectedMeshCountMin) chk(static_cast<int>(r.meshCount) >= *e.expectedMeshCountMin, "meshCount esperado>=" + std::to_string(*e.expectedMeshCountMin) + ", atual=" + std::to_string(r.meshCount));
        if (e.expectedMaterialCountExact) chk(static_cast<int>(r.materialCount) == *e.expectedMaterialCountExact, "materialCount esperado==" + std::to_string(*e.expectedMaterialCountExact) + ", atual=" + std::to_string(r.materialCount));
        if (e.expectedMaterialCountMin) chk(static_cast<int>(r.materialCount) >= *e.expectedMaterialCountMin, "materialCount esperado>=" + std::to_string(*e.expectedMaterialCountMin) + ", atual=" + std::to_string(r.materialCount));
        if (e.expectedAnimationCountExact) chk(static_cast<int>(r.animationCount) == *e.expectedAnimationCountExact, "animationCount esperado==" + std::to_string(*e.expectedAnimationCountExact) + ", atual=" + std::to_string(r.animationCount));
        if (e.expectedAnimationCountMin) chk(static_cast<int>(r.animationCount) >= *e.expectedAnimationCountMin, "animationCount esperado>=" + std::to_string(*e.expectedAnimationCountMin) + ", atual=" + std::to_string(r.animationCount));
        chk((r.skeletonCount > 0) == e.expectSkeleton, std::string("skeleton esperado=") + (e.expectSkeleton?"true":"false") + ", atual=" + (r.skeletonCount>0?"true":"false"));

        std::size_t channelCount = 0, minKeyframes = std::numeric_limits<std::size_t>::max();
        bool durationPositive = true;
        for (const auto& clip : result.data.animationClips) {
            durationPositive = durationPositive && (clip.duration() > 0.0F);
            for (const auto& track : clip.tracks()) {
                channelCount += track.channels.size();
                for (const auto& ch : track.channels) minKeyframes = std::min(minKeyframes, ch.keyframes.size());
            }
        }
        std::size_t boneCount = 0; for (const auto& sk : result.data.skeletons) boneCount += sk.hierarchy().bones().size();
        if (e.expectedChannelCountMin) chk(static_cast<int>(channelCount) >= *e.expectedChannelCountMin, "channels esperado>=" + std::to_string(*e.expectedChannelCountMin) + ", atual=" + std::to_string(channelCount));
        if (e.expectedBoneCountMin) chk(static_cast<int>(boneCount) >= *e.expectedBoneCountMin, "bones esperado>=" + std::to_string(*e.expectedBoneCountMin) + ", atual=" + std::to_string(boneCount));
        if (e.expectedKeyframesPerChannelMin) chk(minKeyframes != std::numeric_limits<std::size_t>::max() && static_cast<int>(minKeyframes) >= *e.expectedKeyframesPerChannelMin, "min keyframes/canal esperado>=" + std::to_string(*e.expectedKeyframesPerChannelMin));
        if (e.expectDurationPositive) chk(durationPositive, "duração de animação esperada > 0");

        for (const auto& w : e.expectedWarningsContains) chk(hasWarning(r.warnings, w), "warning esperado contendo: " + w);
        for (const auto& f : e.expectedFallbackFlags) {
            if (f == "missingDiffuseFallback") chk(hasWarning(r.warnings, "missing diffuse texture"), "fallback missingDiffuseFallback ausente");
            else if (f == "missingUvFallback") chk(hasWarning(r.warnings, "no UV0"), "fallback missingUvFallback ausente");
            else if (f == "missingNormalFallback") chk(hasWarning(r.warnings, "no normals"), "fallback missingNormalFallback ausente");
            else if (f == "defaultMaterialInjected") chk(hasWarning(r.warnings, "default engine material injected"), "fallback defaultMaterialInjected ausente");
        }

        const bool ok = diffs.empty();
        std::cout << "[" << (ok?"PASS":"FAIL") << "] " << e.caseName << " (" << e.format << ")\n";
        for (const auto& d : diffs) std::cout << "  - " << d << "\n";
        if (ok) ++passed; else ++failed;
    }
    std::cout << "Summary: passed=" << passed << " failed=" << failed << "\n";
    return failed == 0 ? 0 : 1;
}
