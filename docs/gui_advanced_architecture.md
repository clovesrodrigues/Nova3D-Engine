# GUI Advanced Architecture

Implemented components in `nova3d::gui`:
- Runtime theme system: `NGUITheme`, `GUIStyleState`, `GUIStyleMetrics`, `NGUISkin`, `GUIThemeManager`, `NGUIStyleResolver`.
- Unified dialogs: `NFileDialog` (open/save/folder), `NColorDialog`, `NMessageBox` with normalized `NDialogResult`.
- Layout stack: `NGUIElementFactory`, `NGUILayoutLoader`, `NGUILayoutSerializer`.
- Advanced widget: `NMeshViewerWidget` with orbit/zoom/hotkeys (`R` reset, `W` wireframe toggle).

Integration path:
1. Sandbox registers widget constructors in factory.
2. Sandbox loads `assets/gui/sandbox_layout.json` at startup.
3. Widgets resolve style state through resolver using theme + style class.
4. Theme switch invalidates skin and repaints same frame path.

Notes:
- Public GUI headers keep backend-agnostic API.
- Native dialog adapters can be added internally later without changing public API.
