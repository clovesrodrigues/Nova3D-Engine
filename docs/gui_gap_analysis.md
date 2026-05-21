# GUI Gap Analysis

## O que já tínhamos
- GUI básica com `GUIWidget`, roteamento de eventos simples de mouse e desenho de quad/texto.
- Layout linear básico (`GUILayout`) e widgets placeholder.

## O que foi implementado agora
- Fundação de tema/skin em runtime: `NGUISkin`, `NGUITheme`, `NGUIStyleSheet`, `NGUIStyleResolver`.
- Diálogos essenciais com API pública unificada (`NFileDialog`, `NColorDialog`, `NMessageBox`) e fallback interno com logs.
- `NMeshViewerWidget` com estado de orbit/zoom/wireframe/normals pronto para integração de renderer.
- Loader serializado inicial em JSON (`NGUILoader`) e factory (`NGUIElementFactory`) para evolução de tipos custom.
- Eventos GUI ampliados para wheel/focus/resize e base para DPI/foco/input callback alignment.

## O que ainda falta
- Backends nativos reais por plataforma para dialogs e clipboard/cursor/IME completos.
- Render real da malha no `NMeshViewerWidget` (viewport offscreen + pipeline dedicado).
- Menus/toolbars/statusbar e layout managers avançados (grid/flex/docking).
- Navegação de gamepad e acessibilidade (focus ring, screen reader hooks, high contrast).
