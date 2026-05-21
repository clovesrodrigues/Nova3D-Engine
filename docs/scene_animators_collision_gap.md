# Scene animators + collision gap

## Antes
Faltavam animators de node padronizados, collision manager de alto nível e selectors de triângulo agregáveis.

## Implementado
- `ISceneNodeAnimator` + `NSceneNodeAnimatorController`.
- `NAnimatorCameraFPS`, `NAnimatorCameraOrbit`, `NAnimatorCollisionResponse`.
- `ISceneCollisionManager` + `NSceneCollisionManager` com ray/screen pick, AABB/Sphere, ground query.
- `ITriangleSelector`, `IMetaTriangleSelector`, `NMeshTriangleSelector`, `NMetaTriangleSelector`.
- Integração no `SceneManager` e demo no Sandbox com log de picking.

## Limitações atuais
- `screenPick` usa ray simplificado em NDC sem inversão completa de matriz.
- `NMeshTriangleSelector` ainda não aplica transformação de mundo dos vértices.
- `NSceneCollisionManager` retorna hit geométrico sem mapear nodeId real.

## Próximos passos
- Sweep tests (capsule/sphere cast), broadphase avançado (BVH dinâmica), character controller completo.
- Nav/crowd e integração com AI runtime.
