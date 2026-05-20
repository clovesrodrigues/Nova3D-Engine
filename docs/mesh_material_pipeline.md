# Mesh + Material Pipeline Foundations

This architecture separates importer CPU data (`ModelAsset` + `MeshCPUData`) from backend GPU resources (`MeshGPUResource`) to keep importer code renderer-agnostic and Vulkan-ready.

- **Model import**: `OBJImporter` parses positions, normals, UVs, indices, `usemtl` groups, and smoothing group tokens into `MeshSection` ranges.
- **Submesh model**: each `MeshSection` stores draw ranges, per-section bounds, and material slot keys.
- **Material instances**: `MaterialTemplate` stores shared shader/state/default parameters; `MaterialInstance` provides per-instance texture/parameter/state overrides.
- **GPU upload flow**: `MeshUploadManager` creates `MeshGPUResource` objects, supports static/dynamic usage tags, and can invalidate/rebuild for streaming and future async upload.
- **Culling + queues**: scene gather performs frustum AABB/sphere checks, then places submissions into opaque/transparent queues with static/dynamic classification.
- **Irrlicht-era comparison**: unlike tightly coupled fixed mesh buffers, this flow introduces explicit CPU/GPU boundaries, section-level material dispatch, and queue architecture for future batching/instancing/descriptor-set based backends.
