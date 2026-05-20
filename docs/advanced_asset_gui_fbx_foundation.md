# Advanced Asset + FBX + GUI Foundation

This foundation introduces a backend-independent tooling/runtime layer for a modernized Nova3D pipeline.

## Asset Pipeline
- `AssetGUID`, `AssetMetadata`, `AssetManifest`, `AssetDatabase`, and `AssetResolver` provide stable IDs, dependency tracking, runtime registration, and lookup by GUID/path/type.
- Metadata JSON serialization is intentionally simple and editor/cooking friendly.

## FBX Strategy
- `FBXImporter` uses `IFBXImporterBackend` abstraction to avoid hard-coupling to Autodesk SDK.
- Backends can be implemented for `ufbx`, `OpenFBX`, or `Assimp` without touching engine-facing runtime APIs.
- Current stub backend validates scene-level import flow for meshes, skeletons, clips, lights, and cameras.

## Animation Asset Pipeline
- `SkeletonAsset`, `AnimationAsset`, `SkinnedMeshAsset` define content-facing containers for imported clips, bind poses, skinning references, and future retargeting/compression hooks.

## GUI Runtime Architecture
- Retained-mode GUI foundation with `GUIManager`, `GUIContext`, `GUIRenderer`, `GUIElement`, `GUIWidget`.
- Core widgets include Window, Panel, Button, Label, TextBox, CheckBox, Slider, ListBox, TreeView (foundational).
- Event routing supports focus/hover/pressed + bubbling and backend-independent input event format.
- `GUILayout` supports horizontal/vertical layout and spacing primitives for future docking/responsive rules.

## Rendering + Font Direction
- GUI rendering is routed through abstract `GUIRenderer` with quad/text hooks and layering.
- Font architecture introduces `Font`, `FontAtlas`, `Glyph`, `TextLayout`-ready metrics for UTF-8 measurement and future shaping/localization.

## Editor Runtime UI Foundation
- `RuntimeUIFoundation` builds hierarchy/inspector/assets/debug windows at runtime as editor-tooling scaffolding.

## Irrlicht-era differences
- Legacy immediate/simple GUI concepts are kept approachable, but now separated into modular layers:
  - input routing,
  - retained widget tree,
  - renderer abstraction,
  - tool-ready windows,
  - and asset database-driven content references.
- This scales to Vulkan-era backends, batching, docking, and editor workflows without backend leakage.
