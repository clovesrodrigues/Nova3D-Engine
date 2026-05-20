# Nova3D Graphics Pipeline Foundation

This document defines the consolidated pipeline: CPU image decode (`TextureLoader`) -> `ImageData` -> backend `ITexture::upload` GPU upload, plus material-driven draw submission and backend render-state caching.

- Asset/backend separation: assets never expose OpenGL types.
- Shader flow: shader assets compile source programs and backend caches uniforms.
- Material flow: `IMaterial` binds shader + texture slots and state before draw.
- Vertex layout: mesh upload now provides semantic layout metadata for flexible future formats.
- State caching: backend suppresses redundant depth/cull/blend/wireframe transitions.
- Render pass base: context frame uses BeginFrame/Clear/EndFrame with stable resize viewport updates.

Compared to classic Irrlicht-era fixed pathways, Nova3D now carries explicit layout/material/state abstractions that map cleanly to future Vulkan pipeline objects.
