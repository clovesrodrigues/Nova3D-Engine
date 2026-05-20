# Nova3D Resource and Asset Foundation

This layer modernizes Irrlicht-style resource management with explicit ownership and backend-agnostic assets.

- `resources::IResource` is the common lifecycle unit (`load/unload/reload`).
- `ResourceCache` stores weak references to avoid circular ownership.
- `ResourceManager` owns orchestration, not global singleton state.
- `io::FileSystem` provides virtualized file access and mounted archives.
- `ArchiveFactory` prepares ZIP/PAK/PK3 mounting; ZIP currently uses minimal directory-backed placeholder behavior.
- Asset classes (`TextureAsset`, `ShaderAsset`, `MeshAsset`, `MaterialAsset`) keep metadata, path tracking, and lazy-load shape.
- `ImporterRegistry` supports extension-based importer dispatch for future plugins.
- Material foundation stores shader reference paths, texture slots, and render states (depth, blend, cull, wireframe).

## Ownership model
- CPU-side assets are `shared_ptr` managed and cache-tracked.
- Cache internally uses `weak_ptr`; once external owners release resources, garbage collection removes stale entries.
- Renderer resources remain behind `graphics::*` interfaces.
- No resource loader directly calls backend-specific APIs.

## Async/hot-reload direction
- API surface keeps `load` and `reload` hooks on each resource.
- Manager/loader are separable so asynchronous job systems can enqueue factory+load stages later.
- No hot-reload watcher is implemented yet.
