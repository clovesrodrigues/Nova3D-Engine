# Forward Rendering Architecture

Nova3D now uses a backend-agnostic forward pipeline with explicit render passes: BeginFrame, ShadowPreparation (placeholder), MainOpaque, Transparent (placeholder), and EndFrame.

## Render targets and framebuffers
- `IRenderTarget` abstracts texture-backed targets and resize.
- OpenGL backend implements framebuffer ownership internally.
- Design is ready for shadow map/depth-only targets and later MRT extension.

## Lighting foundation
- Scene-level light primitives: directional, point, spot.
- `LightSceneNode` feeds light registration into the render pipeline without backend coupling.

## Camera render flow
- Camera controls view/projection/view-projection, viewport, and optional render target assignment.
- Forward renderer executes per-camera with render-pass orchestration.

## Transparency and shadows
- Opaque and transparent queues are separated in scene gather phase.
- Shadow pass is currently a placeholder to keep pass orchestration stable.

## Why this is beyond Irrlicht-era architecture
- Decouples scene traversal from backend commands.
- Encapsulates framebuffer APIs in backend-only code.
- Adds explicit render pass scheduling to support future Vulkan/deferred/PBR expansion.
