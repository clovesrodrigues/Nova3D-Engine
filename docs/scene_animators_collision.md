# Scene Animators + Collision (Fase 4)

## Runtime order
Frame order in `SceneManager`:
1. input (application driven)
2. animator controller update
3. node `onUpdate` hooks
4. world transform propagation
5. gather/visibility
6. render pipeline passes

## Node-centric scene
`ISceneNode` now exposes node-level flags:
- visibility (`setVisible`)
- collision (`setCollisionEnabled`)
- picking (`setPickingEnabled`)

Supported node families in this phase:
- `CameraSceneNode`
- `MeshSceneNode`
- `CharacterSceneNode` (foundation for runtime controller)
- `BillboardSceneNode`
- `LightSceneNode`
- `TerrainSceneNode`

## Animators
- `NAnimatorCameraFPS`: deterministic WASD+look integration using `dt`, speed/sensitivity/pitch clamp setters.
- `NAnimatorCameraOrbit`: Maya-style orbit target, pan, dolly.
- `NAnimatorCollisionResponse`: gravity + basic step offset using ground query.

## Picking and selectors
- `NMeshTriangleSelector`: world-space triangle extraction from mesh nodes.
- `NMetaTriangleSelector`: aggregate selector set.
- `NSceneCollisionManager`: ray/screen pick, AABB/sphere overlap helpers, ground query.

## Import-format compatibility status
The picking/collision/animator stack works for spawned nodes from importer outputs (`spawnModel` path), independent of source format.

Official (validated through importer pipeline):
- 3DS
- B3D
- IRR / IRRMESH
- LWO / LWS
- MD2
- X

Experimental:
- Blender native (`.blend`) is not loaded directly; use intermediate FBX/OBJ/GLTF.
- Any format with partial material/animation parse support may load geometry but with reduced metadata fidelity.


## Camera pivot model
- `CameraSceneNode` now supports a linked `CameraTargetSceneNode` (second pivot).
- Lock mode allows camera + target moving together as one logical object.
- Target can be free, position-driven, or linked to another scene node (ex: character follow).

## Collision manager API
`ISceneCollisionManager` remains the public scene abstraction for ray picking, screen picking, overlap checks and ground queries.
