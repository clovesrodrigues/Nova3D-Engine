# Nova3D Advanced Scene Foundations

This update introduces foundational, backend-independent architecture for skeletal animation, terrain, and spatial partitioning.

- **Skeletal architecture**: `Skeleton`, `BoneHierarchy`, `Bone`, `BoneTransform`, and `BonePose` with bind/inverse-bind data and transform propagation.
- **Animation clips and playback**: `AnimationClip`, `AnimationTrack`, `AnimationChannel`, `Keyframe`, plus `AnimationState`, `AnimationPlayer`, and `AnimationController`.
- **Skinned mesh prep**: mesh vertices now carry bone indices/weights; skinned binding data can store palette upload payloads for future GPU skinning.
- **Terrain foundation**: `HeightmapData`, `TerrainMesh`, `TerrainPatch`, and `HeightmapTerrain` supporting RAW ingest and terrain mesh generation.
- **Spatial partitioning foundation**: abstract `SpatialPartition` with octree/BVH baseline implementations for registration and frustum queries.
- **Scene scalability direction**: `SceneManager` now includes spatial rebuild hooks and a terrain scene node foundation.

## Design philosophy and Irrlicht evolution

Irrlicht-style simplicity is preserved through compact APIs and straightforward data ownership, while scalability is improved via explicit modular boundaries:

- animation domain (`Animation/*`) is decoupled from renderer backend;
- terrain generation (`Terrain/*`) is decoupled from rendering pipeline implementation;
- spatial acceleration (`Spatial/*`) is replaceable via abstraction;
- scene integration uses high-level registrations and visibility flows.

## Forward path

These foundations are intentionally prepared (not fully implemented yet) for:

- GPU matrix palette upload and skinning passes;
- animation blending/layering/state graph logic;
- chunked terrain streaming + LOD;
- richer octree/BVH optimization and streaming-aware ownership;
- Vulkan-ready backend implementations without changing high-level systems.
