#pragma once
#include <string>
#include <unordered_map>
#include <vector>
namespace nova3d::fonts {
struct Glyph { char32_t codepoint{}; float advance=0; float u0=0,v0=0,u1=0,v1=0; int width=0,height=0,bearingX=0,bearingY=0; };
class FontAtlas { public: void setSize(int w,int h){m_w=w;m_h=h;} void addGlyph(Glyph g){m_glyphs[g.codepoint]=g;} const Glyph* find(char32_t cp) const { auto it=m_glyphs.find(cp); return it==m_glyphs.end()?nullptr:&it->second; } private: int m_w=0,m_h=0; std::unordered_map<char32_t,Glyph> m_glyphs; };
class Font { public: explicit Font(std::string n):m_name(std::move(n)){} FontAtlas& atlas(){return m_atlas;} float measureUtf8(const std::string& text) const; private: std::string m_name; FontAtlas m_atlas; };
struct TextLayout { float width=0; std::vector<float> advances; };
}
