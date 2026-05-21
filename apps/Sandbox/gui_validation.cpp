#include <Nova3D/GUI/GUI.hpp>
#include <cassert>
#include <iostream>

int main(){
    using namespace nova3d::gui;
    NGUIElementFactory f;
    f.registerType("button", [](){ return std::make_shared<Button>(); });
    f.registerType("meshviewer", [](){ return std::make_shared<NMeshViewerWidget>(); });
    assert(f.hasType("button"));

    GUIContext ctx; auto skin=std::make_shared<NGUISkin>();
    const bool ok=NGUILayoutLoader::loadLayoutJson("assets/gui/sandbox_layout.json", ctx, f, skin);
    assert(ok);
    assert(!ctx.roots().empty());

    auto dark=NGUITheme::dark(); auto light=NGUITheme::light();
    assert(dark.colors.at("widget.normal").x != light.colors.at("widget.normal").x);

    auto fd=NFileDialog::open("Open","*.obj", false);
    assert(fd.accepted && !fd.selectedPaths.empty());

    NMeshViewerWidget vw; vw.rect={0,0,100,100};
    GUIInputEvent wh{}; wh.type=GUIInputEvent::Type::MouseWheel; wh.x=10; wh.y=10; wh.wheelY=1; vw.handle(wh);
    assert(vw.state.zoom>1.0F);

    std::cout << "gui_validation: OK\n";
    return 0;
}
