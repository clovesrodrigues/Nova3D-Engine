# Import Regression Suite

## Visão geral
A suíte `nova_import_regression` valida o pipeline real `NModelImporter` contra manifestos de expectativa em `assets/tests/import/expected/*.json`.

## Estrutura
- `assets/tests/import/valid/`: casos que devem importar com sucesso.
- `assets/tests/import/invalid/`: arquivos quebrados/ilegíveis (falha esperada).
- `assets/tests/import/edgecases/`: entradas controladas para fallback/warnings.
- `assets/tests/import/expected/`: manifesto esperado vs real.

## Formato do manifesto
Campos mínimos:
- `caseName`, `format`, `asset`, `expectedSuccess`.

Campos suportados:
- `expectedNodeCountMin|Exact`
- `expectedMeshCountMin|Exact`
- `expectedMaterialCountMin|Exact`
- `expectedAnimationCountMin|Exact`
- `expectSkeleton`
- `expectedChannelCountMin`
- `expectedBoneCountMin`
- `expectedKeyframesPerChannelMin`
- `expectDurationPositive`
- `expectedWarningsContains` (lista de substrings)
- `expectedFallbackFlags`: `missingDiffuseFallback`, `missingUvFallback`, `missingNormalFallback`, `defaultMaterialInjected`

## Execução local
```bash
cmake -S . -B build
cmake --build build --target nova_import_regression
./build/apps/AssetValidator/nova_import_regression assets/tests/import all
ctest --test-dir build --output-on-failure -R import_regression_
cmake --build build --target check_import_regression
```

## Interpretação de falhas
Cada caso imprime `PASS/FAIL` e diffs estruturados (ex.: `meshCount esperado>=1, atual=0`). O processo retorna código não-zero quando houver falhas.

## Official vs Experimental
- **Official**: formatos com caso válido + inválido passando consistentemente em CI.
- **Experimental**: formatos apenas com falha esperada ou sem cobertura de fallback/animação.

Política recomendada: mover formato para Official somente com pelo menos 1 caso válido estruturado e 1 edgecase de fallback cobertos.
