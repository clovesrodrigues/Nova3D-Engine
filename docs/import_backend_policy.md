# Política oficial de backend de import

## Princípios
- A API pública Nova3D **não expõe tipos Assimp**.
- Assimp é backend interno via adapter (`NAssimpImporterBackend` ou equivalente interno).
- O contrato público permanece em tipos Nova3D (`NModelData`, `NImportReport`, `NMeshData`, etc.).

## Regras de encapsulamento
- Headers públicos em `include/Nova3D/...` não devem incluir headers `assimp/*`.
- Conversão de estruturas Assimp -> Nova3D ocorre apenas em implementação (`src/...`).
- Erros internos de backend devem ser mapeados para relatórios Nova3D estáveis.

## Estratégia de swap de backend
- Importers públicos chamam abstração de backend (interface interna), não Assimp diretamente no contrato público.
- Novo backend (ex.: glTF dedicado) deve preservar os mesmos tipos públicos e semântica de retorno.
- Alterações de comportamento por backend devem ser versionadas e documentadas como capability.

## Versionamento do backend de import
- Versão semântica da SDK governa o contrato público.
- Upgrade de Assimp que não altera tipos públicos => mudança interna (patch/minor conforme impacto).
- Qualquer mudança observável de output (materiais, hierarquia, animação) exige nota de release com comparação por formato.
