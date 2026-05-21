#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <optional>
#include <Nova3D/Core/Math/Math.hpp>

namespace nova3d::gui {
struct GUIInputEvent { enum class Type{MouseMove,MouseDown,MouseUp,KeyDown,KeyUp,TextInput,MouseWheel,WindowResized,FocusChanged}; Type type{}; int x=0,y=0,key=0; int wheelY=0; bool focused=false; char32_t codepoint=0; };
struct Rect{float x=0,y=0,w=0,h=0; bool contains(float px,float py) const{return px>=x&&py>=y&&px<=x+w&&py<=y+h;}};

struct NGUITheme { std::string name{"dark"}; std::unordered_map<std::string, math::Vector4> colors; std::unordered_map<std::string, float> metrics; std::unordered_map<std::string, std::string> fonts; static NGUITheme dark(); static NGUITheme light();};
struct NGUIStyleSheet { std::unordered_map<std::string, std::unordered_map<std::string, math::Vector4>> classColors; };
class NGUIStyleResolver { public: math::Vector4 color(const NGUITheme& theme, const NGUIStyleSheet& sheet, const std::string& styleClass, const std::string& key, const math::Vector4& fallback) const; };
class NGUISkin { public: void setTheme(NGUITheme theme); const NGUITheme& theme() const{return m_theme;} void setStyleSheet(NGUIStyleSheet s){m_sheet=std::move(s);} const NGUIStyleSheet& styleSheet() const{return m_sheet;} void addInvalidationListener(std::function<void()> cb){m_listeners.push_back(std::move(cb));} private: NGUITheme m_theme=NGUITheme::dark(); NGUIStyleSheet m_sheet{}; std::vector<std::function<void()>> m_listeners;};

class GUIRenderer { public: virtual ~GUIRenderer()=default; virtual void beginFrame()=0; virtual void drawQuad(const Rect&, const math::Vector4&, int layer)=0; virtual void drawText(const Rect&, const std::string&, const math::Vector4&, int layer)=0; virtual void endFrame()=0; };
class GUIElement { public: virtual ~GUIElement()=default; void addChild(std::shared_ptr<GUIElement> c){m_children.push_back(std::move(c));} virtual void draw(GUIRenderer& r); virtual bool handle(const GUIInputEvent& ev); Rect rect{}; bool visible=true,enabled=true,focused=false; std::string id; std::string styleClass; protected: std::vector<std::shared_ptr<GUIElement>> m_children; };
class GUIWidget: public GUIElement { public: std::function<void()> onClick; std::string text; bool hovered=false,pressed=false; bool handle(const GUIInputEvent& ev) override; void draw(GUIRenderer& r) override; void setSkin(std::shared_ptr<NGUISkin> skin){m_skin=std::move(skin);} protected: std::shared_ptr<NGUISkin> m_skin;};

struct NMeshViewerState { std::string meshPath; float orbitYaw=0; float orbitPitch=0; float zoom=1.0F; bool wireframe=false; bool normals=false; math::Vector4 tint{0.8F,0.8F,0.9F,1};};
class NMeshViewerWidget final : public GUIWidget { public: NMeshViewerState state; bool handle(const GUIInputEvent& ev) override; void draw(GUIRenderer& r) override; };

class GUIContext { public: void addRoot(std::shared_ptr<GUIElement> e){m_root.push_back(std::move(e));} bool route(const GUIInputEvent& ev); void draw(GUIRenderer& r); std::vector<std::shared_ptr<GUIElement>>& roots(){return m_root;} private: std::vector<std::shared_ptr<GUIElement>> m_root; };
class GUIManager { public: explicit GUIManager(std::shared_ptr<GUIRenderer> r):m_renderer(std::move(r)){} GUIContext& context(){return m_ctx;} bool process(const GUIInputEvent& ev){return m_ctx.route(ev);} void setSkin(std::shared_ptr<NGUISkin> skin){m_skin=std::move(skin);} std::shared_ptr<NGUISkin> skin() const{return m_skin;} void tick(); private: GUIContext m_ctx; std::shared_ptr<GUIRenderer> m_renderer; std::shared_ptr<NGUISkin> m_skin; };

struct NDialogResult { bool accepted=false; std::vector<std::string> paths; math::Vector4 color{1,1,1,1}; int button=0;};
class NFileDialog { public: static NDialogResult open(const std::string& title,const std::string& filter,bool multiSelect=false); static NDialogResult save(const std::string& title,const std::string& filter); };
class NColorDialog { public: static NDialogResult pick(const math::Vector4& seed); };
class NMessageBox { public: enum class Type{Info,Warn,Error}; enum class Buttons{Ok,YesNo}; static NDialogResult show(const std::string& title,const std::string& message, Type type, Buttons buttons);};

class Window: public GUIWidget{}; class Panel: public GUIWidget{}; class Button: public GUIWidget{}; class Label: public GUIWidget{}; class TextBox: public GUIWidget{}; class CheckBox: public GUIWidget{}; class Slider: public GUIWidget{}; class ListBox: public GUIWidget{}; class TreeView: public GUIWidget{};
class GUILayout { public: enum class Mode{Vertical,Horizontal}; explicit GUILayout(Mode m):mode(m){} void apply(std::vector<std::shared_ptr<GUIElement>>& children, const Rect& bounds,float padding=4.0F) const; Mode mode;};

class NGUIElementFactory { public: using Creator=std::function<std::shared_ptr<GUIElement>()>; void registerType(const std::string& type, Creator creator); std::shared_ptr<GUIElement> create(const std::string& type) const;};
class NGUILoader { public: static bool loadLayoutJson(const std::string& path, GUIContext& ctx, NGUIElementFactory& factory, std::shared_ptr<NGUISkin> skin);};

}
