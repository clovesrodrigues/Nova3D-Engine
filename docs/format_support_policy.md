# Política de suporte de formatos

## Fase 1 (suporte oficial inicial)

| Formato | Status | Escopo |
|---|---|---|
| OBJ | **Official** | Geometria estática + materiais básicos.
| FBX | **Official** | Geometria, hierarquia básica e caminhos de evolução para animação.
| glTF/GLB | **Official** | Intercâmbio moderno com materiais PBR conforme cobertura atual do importer.

## Fase 2 (expansão controlada)

| Formato | Status inicial |
|---|---|
| PLY | Experimental |
| STL | Experimental |
| 3DS | Not guaranteed |
| DXF | Not guaranteed |
| X | Not guaranteed |
| MD2 / MD3 / MD5 | Not guaranteed |
| Ogre XML | Not guaranteed |

## Critérios para promover Experimental -> Official
- [ ] Conjunto de assets de teste versionado no repositório (mínimo feliz + casos de borda).
- [ ] Validação automatizada no `AssetValidator` ou suíte equivalente CI.
- [ ] Cobertura mínima documentada de materiais e, quando aplicável, animação/esqueleto.
- [ ] Resultado consistente em pelo menos 2 targets da matriz de build.

## Limite de promessa
Nova3D **não** promete “abrir qualquer formato”. O suporte é progressivo, validado por fase e sujeito à política acima.
