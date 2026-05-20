#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <Nova3D/Core/Math/Math.hpp>

namespace nova3d::gui {
struct GUIInputEvent { enum class Type{MouseMove,MouseDown,MouseUp,KeyDown,KeyUp,TextInput}; Type type{}; int x=0,y=0,key=0; char32_t codepoint=0; };
struct Rect{float x=0,y=0,w=0,h=0; bool contains(float px,float py) const{return px>=x&&py>=y&&px<=x+w&&py<=y+h;}};
class GUIRenderer { public: virtual ~GUIRenderer()=default; virtual void beginFrame()=0; virtual void drawQuad(const Rect&, const math::Vector4&, int layer)=0; virtual void drawText(const Rect&, const std::string&, const math::Vector4&, int layer)=0; virtual void endFrame()=0; };
class GUIElement { public: virtual ~GUIElement()=default; void addChild(std::shared_ptr<GUIElement> c){m_children.push_back(std::move(c));} virtual void draw(GUIRenderer& r); virtual bool handle(const GUIInputEvent& ev); Rect rect{}; bool visible=true,enabled=true,focused=false; protected: std::vector<std::shared_ptr<GUIElement>> m_children; };
class GUIWidget: public GUIElement { public: std::function<void()> onClick; std::string text; bool hovered=false,pressed=false; bool handle(const GUIInputEvent& ev) override; void draw(GUIRenderer& r) override; };
class GUIContext { public: void addRoot(std::shared_ptr<GUIElement> e){m_root.push_back(std::move(e));} bool route(const GUIInputEvent& ev); void draw(GUIRenderer& r); private: std::vector<std::shared_ptr<GUIElement>> m_root; };
class GUIManager { public: explicit GUIManager(std::shared_ptr<GUIRenderer> r):m_renderer(std::move(r)){} GUIContext& context(){return m_ctx;} void tick(); bool process(const GUIInputEvent& ev){return m_ctx.route(ev);} private: GUIContext m_ctx; std::shared_ptr<GUIRenderer> m_renderer; };
class Window: public GUIWidget{}; class Panel: public GUIWidget{}; class Button: public GUIWidget{}; class Label: public GUIWidget{}; class TextBox: public GUIWidget{}; class CheckBox: public GUIWidget{}; class Slider: public GUIWidget{}; class ListBox: public GUIWidget{}; class TreeView: public GUIWidget{};
class GUILayout { public: enum class Mode{Vertical,Horizontal}; explicit GUILayout(Mode m):mode(m){} void apply(std::vector<std::shared_ptr<GUIElement>>& children, const Rect& bounds,float padding=4.0F) const; Mode mode;};
}
