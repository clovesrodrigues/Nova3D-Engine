#pragma once
#include <Nova3D/GUI/GUI.hpp>
namespace nova3d::editor {
struct RuntimeUIFoundation { std::shared_ptr<gui::Window> hierarchy; std::shared_ptr<gui::Window> inspector; std::shared_ptr<gui::Window> assetBrowser; std::shared_ptr<gui::Panel> debugOverlay; static RuntimeUIFoundation build(gui::GUIContext& ctx); };
}
