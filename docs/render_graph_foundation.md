# Nova3D Render Graph Foundation (Pass-Based, Incremental)

## 1) Render Graph Architecture Overview

### Visão geral
A **Render Graph Foundation** da Nova3D é um sistema pass-based que organiza a renderização por:
- **passes** (unidades explícitas de trabalho),
- **resources** (texturas/buffers/targets abstratos),
- **handles** (referências estáveis sem ponteiros crus de backend),
- **dependencies** (leituras/escritas explícitas),
- **execution stages** (ordem de execução compilada por frame).

O foco não é um “AAA frame graph compiler”, e sim uma base sólida para:
- integrar o forward renderer atual,
- remover acoplamentos implícitos,
- preparar transição futura para Vulkan,
- habilitar evolução incremental (post-process, shadows, HDR, transient allocator, MT).

### Objetivos
1. Separar claramente **runtime** e **render backend**.
2. Tornar dependências de recursos **explícitas** no frame.
3. Garantir independência de backend no nível de alto/médio nível.
4. Preservar APIs públicas atuais (adaptação incremental).
5. Preparar modelo de estados de recurso compatível com APIs explícitas (Vulkan).
6. Criar pontos formais para debug/profiling/tooling.

### Non-goals (fase foundation)
- Não implementar async compute.
- Não implementar compilador avançado de barreiras.
- Não implementar pass merging automático.
- Não reescrever todo o renderer.
- Não tornar runtime “renderer-driven”.
- Não introduzir editor visual de graph nesta etapa.

### Responsabilidades do sistema
- Capturar passes e declarações R/W no Build.
- Resolver ordem/dependências e plano de recursos no Compile.
- Executar passes via backend abstraction no Execute.
- Expor metadados de inspeção para tooling/debug.

### Limites do sistema
Render Graph **não** deve operar diretamente sobre:
- scene graph,
- gameplay,
- ECS,
- SDL,
- objetos OpenGL brutos (GLuint, FBO, etc.) em níveis altos.

Ele opera sobre contratos de renderização: `FrameRenderData`, `PassDescriptor`, `ResourceHandle`, `BackendCommandContext`.

---

## 2) Frame Lifecycle Design

Pipeline de frame recomendado (foundation-first):

1. **Frame Begin**
   - reset de allocators frame-local,
   - begin de profiling scopes,
   - aquisição de swapchain/backbuffer handle abstrato.

2. **Update (Runtime)**
   - gameplay/systems/scene updates,
   - sem dependência direta de backend gráfico.

3. **Animation**
   - skinning prep, pose update, culling data update.

4. **Visibility Extraction**
   - culling + coleta de visibilidade em estruturas de render extraction.

5. **Render Extraction**
   - transformar scene/runtime state em `FrameRenderData` **imutável** para render.

6. **Render Graph Build**
   - registrar passes com inputs/outputs explícitos,
   - declarar resources necessários (persistent/frame-local/transient).

7. **Render Graph Compile**
   - topological ordering,
   - validações de uso,
   - inferência de estados conceituais,
   - plano de aliasing/transient (inicialmente simples, sem otimizações agressivas).

8. **Render Graph Execute**
   - executar passes na ordem compilada,
   - emitir markers/profiling hooks,
   - backend resolve binding físico dos handles.

9. **UI Rendering**
   - pass específico de UI no graph (ou integração transitória ao final),
   - manter separação de responsabilidades.

10. **Present**
   - transição conceitual para `Present` do backbuffer,
   - submit/present no backend.

11. **Frame End**
   - fechar scopes de profiling,
   - coletar telemetria/frame stats,
   - liberar frame-local/transient.

**Diretriz crítica:** runtime termina em `FrameRenderData`; dali em diante o pipeline é render domain.

---

## 3) Render Pass System Design

### Abstração de pass
Cada pass é definido por:
- identidade (`PassId`, nome debug),
- estágio lógico (`Opaque`, `Transparent`, `Shadow`, `Post`, `UI`, etc.),
- declarações de leitura/escrita de resources,
- callback/functor de execução backend-agnostic.

### Pass Descriptor (conceitual)
Campos recomendados:
- `name`, `stage`, `flags`,
- `reads[]` e `writes[]` com `ResourceHandle + AccessType`,
- parâmetros de execução (viewport/scissor/clear/load/store conceituais),
- ponteiro/referência para payload imutável do frame.

### Inputs/Outputs explícitos
- Input = handle + modo de acesso de leitura.
- Output = handle + modo de escrita/attachment.
- Proibir uso implícito de globals não declarados.

### Scheduling
- DAG por dependências de recursos.
- Ordenação topológica base.
- Estágios lógicos guiam ordem preferencial quando não há dependência direta.

### Compile phase
- validação de ciclos,
- validação de múltiplos writers inválidos no mesmo escopo temporal,
- resolução de producer/consumer,
- inserção de transições conceituais de estado (metadado).

### Execute phase
- loop linear de passes compilados,
- cada pass recebe `BackendCommandContext` + handles resolvidos,
- backend mapeia abstrações para OpenGL atual;
- design preserva caminho para mapping Vulkan posterior.

---

## 4) Resource System Design

### Tipos de handles
- `TextureHandle`
- `BufferHandle`
- `RenderTargetHandle` (pode ser wrapper semântico sobre texture/depth attachments)

Handles devem ser:
- pequenos (inteiro/índice + geração),
- estáveis durante o frame,
- inválidos fora de escopo semântico adequado.

### Categorias de recursos
1. **Persistent resources**
   - sobrevivem múltiplos frames (ex.: shadow atlas futuro, LUTs, buffers globais).
2. **Frame-local resources**
   - válidos apenas no frame corrente.
3. **Transient resources**
   - alocados/aliased automaticamente em janela de vida curta entre passes.

### Ownership model
- Alto nível “possui” apenas handles e descritores lógicos.
- Backend/resource allocator possui objetos físicos.
- Graph compile decide lifetime lógico; allocator executa política concreta.

### Lifetime model
- início de vida: primeira escrita ou import explícito.
- fim de vida: último uso conhecido no graph compilado.
- transient aliasing: permitido somente quando lifetimes não se sobrepõem.

---

## 5) Resource State Model (conceitual)

Estados mínimos recomendados:
- `Read`
- `Write`
- `ColorAttachment`
- `DepthAttachment`
- `ShaderRead`
- `TransferSrc`
- `TransferDst`
- `Present`

Mesmo em OpenGL (estado implícito), manter esse modelo como metadado é essencial para:
- validação de uso,
- debug,
- portabilidade para Vulkan (onde estados/barreiras são explícitos).

**Regra:** o graph opera no modelo conceitual; backend traduz para a API corrente.

---

## 6) Integration Strategy (incremental, sem quebra)

### Integração com renderer atual
- Introduzir Render Graph como camada de orquestração acima do backend atual.
- Forward renderer vira inicialmente um conjunto reduzido de passes (ex.: depth-prepass opcional, opaque forward, transparent, sky, UI).

### Integração com forward renderer atual
- Fase 1: encapsular sequência atual em 1 pass macro (`ForwardMainPass`) para adoção sem ruptura.
- Fase 2: particionar gradualmente em passes menores com declarações R/W.

### Integração com material system
- Material continua dono de shaders/params.
- Pass apenas agenda draw lists já extraídas.
- Sem introduzir dependência material->backend específico fora da abstração existente.

### Integração com render queue atual
- Render queue permanece fonte de draw commands para passes.
- Passes consomem filas por estágio (opaque/transparent/ui) via `FrameRenderData`.

### Integração com scene extraction atual
- Adicionar etapa formal de `Render Extraction` para produzir snapshot imutável.
- Evitar acesso direto do pass ao scene graph runtime.

---

## 7) Migration Strategy

### Fase A — Foundation mínima
- tipos base: handles, descriptors, pass builder, graph builder,
- pipeline Build/Compile/Execute funcional,
- 1 macro pass integrado ao forward atual.

### Fase B — Decomposição controlada
- separar `ForwardMainPass` em passes explícitos,
- declarar backbuffer/depth como resources importados,
- habilitar validações de acesso.

### Fase C — Recursos transitórios iniciais
- introduzir transient texture handles para pós-process futuro,
- allocator simples linear/frame arena.

### Fase D — Expansões
- shadow passes,
- post-process chain,
- HDR pipeline.

### Critério de sucesso por fase
- nenhuma quebra de API pública,
- paridade visual com renderer atual,
- melhoria mensurável de observabilidade (debug/profiling).

---

## 8) Threading Preparation Strategy

Mesmo single-thread inicialmente, preparar:

1. **Immutable FrameRenderData**
   - produzido por extraction;
   - consumido por render sem escrita concorrente.

2. **Render thread future-ready**
   - Build/Compile podem migrar para thread de render dedicada.

3. **Command recording preparation**
   - Execute separado em “record abstractions” e “backend submit”.

4. **Data ownership claro**
   - runtime não muta dados consumidos por passes em execução.

5. **Determinismo**
   - graph compilado determinístico por frame para facilitar replay/debug.

---

## 9) Debug / Profiling Strategy

### Debug markers
- cada pass injeta marker com nome estável,
- incluir IDs de recursos principais no metadado de debug.

### Pass visualization
- export do DAG compilado (JSON simples ou DOT) por frame/snapshot.
- overlay opcional: ordem de passes + custos.

### Profiling hooks
- CPU timing por fase (Build/Compile/Execute) e por pass.
- interface para GPU timing futuro (timestamp queries / Vulkan timestamps).

### Frame capture preparation
- pontos de hook para capturadores (RenderDoc etc.) sem contaminar alto nível.
- registrar metadata amigável (nomes de pass/resources) para inspeção.

---

## 10) Vulkan Preparation Analysis

### Decisões atuais que ajudam Vulkan
1. Handles abstratos ao invés de objetos GL expostos.
2. Estados conceituais explícitos de recurso.
3. Passes com declarações R/W obrigatórias.
4. Separação Build/Compile/Execute.
5. Frame data imutável e separação runtime/render.

### Decisões a evitar (riscos de incompatibilidade)
1. Dependência em side effects implícitos de OpenGL.
2. Passes que acessam recursos não declarados.
3. Globals de estado mutável compartilhado entre passes.
4. Misturar lógica de gameplay dentro de passes.
5. API de pass acoplada a conceitos específicos de FBO/GL state machine.

### Abstrações que devem permanecer explícitas
- Access mode por recurso em cada pass.
- Lifetime lógico de recurso.
- Origem de recurso (importado, transient, persistent).
- Fronteiras de estágio e ordem compilada.
- Metadados de sincronização (mesmo que noop em GL).

---

## Riscos arquiteturais e armadilhas comuns

1. **“Graph fake” sem declaração real de dependências**
   - risco: graph vira só lista linear manual.
   - mitigação: exigir reads/writes explícitos no descriptor.

2. **Acoplamento do graph ao backend OpenGL**
   - risco: bloqueio de Vulkan.
   - mitigação: nenhum tipo GL em interfaces de alto nível.

3. **Compile phase fraca/inexistente**
   - risco: erros só aparecem em runtime com artefatos difíceis.
   - mitigação: validações fortes (ciclos, uso após free, writer conflict).

4. **Migração big-bang**
   - risco: regressão alta e perda de produtividade.
   - mitigação: macro pass inicial + decomposição progressiva.

5. **Estado global oculto em passes**
   - risco: comportamento não determinístico.
   - mitigação: payload imutável + declarações explícitas + debug markers.

6. **Overengineering precoce**
   - risco: complexidade sem ganho prático.
   - mitigação: foundation mínima operacional antes de otimizações avançadas.

7. **Runtime contaminando render domain**
   - risco: threading futuro inviável.
   - mitigação: fronteira formal `Render Extraction -> FrameRenderData`.

---

## Plano de adoção recomendado (resumo executivo)

1. Introduzir estruturas base do graph e ciclo Build/Compile/Execute.
2. Encapsular forward atual em macro pass sem quebrar API.
3. Tornar recursos de backbuffer/depth explícitos via handles importados.
4. Adicionar validações e debug/profiling hooks.
5. Decompor passes gradualmente para habilitar post/shadows/HDR.
6. Só então evoluir para transient allocator avançado e MT recording.

Essa trajetória entrega valor imediato (clareza/observabilidade) e preserva compatibilidade futura com Vulkan sem reescrever a engine.
