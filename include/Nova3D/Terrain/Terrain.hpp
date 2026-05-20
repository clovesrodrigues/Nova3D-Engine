#pragma once
#include <memory>
#include <vector>
#include <Nova3D/Scene/MeshData.hpp>
namespace nova3d::terrain {
struct HeightmapData { std::uint32_t width=0,height=0; std::vector<float> heights; float sample(std::uint32_t x,std::uint32_t y) const; };
class TerrainMesh { public: scene::MeshCPUData generateFromHeightmap(const HeightmapData& hm, float scaleXZ, float scaleY) const; };
struct TerrainPatch { std::uint32_t patchX=0,patchY=0,size=0; math::AABB bounds{}; scene::MeshCPUData mesh{}; };
class HeightmapTerrain { public: bool loadHeightmapImage(const std::string& path); bool loadHeightmapRaw(const std::string& path, std::uint32_t w, std::uint32_t h); void buildPatches(std::uint32_t patchSize, float scaleXZ, float scaleY); const std::vector<TerrainPatch>& patches() const { return m_patches; } const HeightmapData& heightmap() const { return m_heightmap; } private: HeightmapData m_heightmap; std::vector<TerrainPatch> m_patches; };
}
