#include <Nova3D/Fonts/Font.hpp>
namespace nova3d::fonts { float Font::measureUtf8(const std::string& text) const { float w=0; for(unsigned char c: text){ if(auto* g=m_atlas.find(c)) w+=g->advance; else w+=8.0F; } return w; } }
