# Nova3D Tooling Foundations

This document introduces modular foundations for serialization, runtime reflection, prefab workflows, debug visualization, runtime introspection, object registry tracking, dependency graphs, and hot-reload hooks.

## Why this differs from Irrlicht-era limits

Historically, scene systems were tightly coupled to runtime rendering and ad-hoc data formats. Nova3D now separates concerns through backend-independent runtime APIs so editor tooling, serialization backends, and future scripting can evolve independently.

## Foundations added

- Serialization core (`Serializer`, `Deserializer`, `SerializableObject`, `SerializationContext`) with versioning and delta/network flags.
- JSON backend (`JsonSerializer`/`JsonDeserializer`) as current format, designed for YAML/binary extension.
- Scene serialization (`SceneSerializer`, `SceneLoader`) for scene graph persistence scaffolding.
- Prefab architecture (`Prefab`, `PrefabInstance`, `PrefabSerializer`) with override maps.
- Runtime reflection metadata (`TypeInfo`, `PropertyInfo`, `MethodInfo`, `EnumInfo`, `ObjectFactory`, `TypeRegistry`).
- Runtime object registry (`ObjectRegistry`) with stable IDs and weak lookup.
- Debug visualization (`DebugRenderer`) with persistent/frame primitives and frustum/bounds helpers.
- Runtime introspection entry points (`RuntimeIntrospection`) for editor inspectors.
- Dependency graph and hot-reload prep (`DependencyGraph`, `HotReloadHooks`) for asset invalidation and notifications.

## Future direction

These systems are intentionally renderer-agnostic and platform-independent to remain Android-ready and Vulkan-ready while preserving existing abstractions. They establish minimal but scalable interfaces for future editor UI, scripting integration, dependency tooling, scene streaming, and robust hot reload.
