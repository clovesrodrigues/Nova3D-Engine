# Nova3D Math + Scene Foundations

- Coordinate system: right-handed; +X right, +Y up, -Z forward (OpenGL style).
- Matrix layout: column-major storage; vectors are treated as column vectors (`M * v`).
- Transform composition: `T * R * S` local composition, world as `parent * local`.
- Scene graph: Irrlicht-inspired parent/child nodes with modern C++ ownership (`shared_ptr` children, `weak_ptr` parent).
- Renderer independence: scene nodes never call OpenGL directly and only submit via `IVideoDriver`.
- Camera: perspective/orthographic, look-at view generation, frustum extraction for visibility checks.
- Render queue: minimal submit list for future sorting/batching/deferred extensions.

Compared to legacy Irrlicht, this design keeps the familiar node-centric traversal but enforces backend abstraction boundaries and explicit math conventions to stay Vulkan-ready and Android-ready.
