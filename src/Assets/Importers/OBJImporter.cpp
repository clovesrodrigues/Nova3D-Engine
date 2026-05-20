#include <Nova3D/Assets/Importers/OBJImporter.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>
namespace nova3d::assets {
static math::AABB boundsFrom(const std::vector<scene::MeshVertex>& v,std::uint32_t start,std::uint32_t count,const std::vector<std::uint32_t>& idx){ math::AABB b{{1e9F,1e9F,1e9F},{-1e9F,-1e9F,-1e9F}}; for(std::uint32_t i=0;i<count;++i){ auto p=v[idx[start+i]].position; b.min.x=std::min(b.min.x,p.x); b.min.y=std::min(b.min.y,p.y); b.min.z=std::min(b.min.z,p.z); b.max.x=std::max(b.max.x,p.x); b.max.y=std::max(b.max.y,p.y); b.max.z=std::max(b.max.z,p.z);} return b; }
bool OBJImporter::canImport(const std::string& extension) const { return extension=="obj"||extension==".obj"; }
std::shared_ptr<ModelAsset> OBJImporter::importFromFile(const std::string& path) const {
    std::ifstream f(path); if(!f) return {};
    auto out=std::make_shared<ModelAsset>(); std::vector<math::Vector3> p,n; std::vector<math::Vector2> t; std::string line; scene::MeshSection section{}; section.materialSlot="default";
    while(std::getline(f,line)){ std::istringstream ss(line); std::string tok; ss>>tok; if(tok=="v"){ math::Vector3 v{}; ss>>v.x>>v.y>>v.z; p.push_back(v);} else if(tok=="vn"){ math::Vector3 v{}; ss>>v.x>>v.y>>v.z; n.push_back(v);} else if(tok=="vt"){ math::Vector2 uv{}; ss>>uv.x>>uv.y; t.push_back(uv);} else if(tok=="usemtl"){ if(section.indexCount>0){ section.bounds=boundsFrom(out->mesh.vertices,section.indexOffset,section.indexCount,out->mesh.indices); out->mesh.sections.push_back(section);} section={}; ss>>section.materialSlot; section.indexOffset=(uint32_t)out->mesh.indices.size(); } else if(tok=="s"){ ss>>section.smoothingGroup; } else if(tok=="mtllib"){ std::string mtl; ss>>mtl; out->materials["default"].name="default"; out->materials["default"].diffuseColor={1,1,1}; }
        else if(tok=="f"){ for(int i=0;i<3;++i){ std::string vi; ss>>vi; std::replace(vi.begin(),vi.end(),'/', ' '); std::istringstream vs(vi); int pi=0,ti=0,ni=0; vs>>pi>>ti>>ni; scene::MeshVertex mv{}; mv.position=p[(size_t)(pi-1)]; if(ti>0) mv.uv=t[(size_t)(ti-1)]; if(ni>0) mv.normal=n[(size_t)(ni-1)]; out->mesh.vertices.push_back(mv); out->mesh.indices.push_back((uint32_t)out->mesh.vertices.size()-1); section.indexCount++; } }
    }
    if(section.indexCount>0){ section.bounds=boundsFrom(out->mesh.vertices,section.indexOffset,section.indexCount,out->mesh.indices); out->mesh.sections.push_back(section); }
    if(!out->mesh.vertices.empty()){ out->mesh.bounds=boundsFrom(out->mesh.vertices,0,(uint32_t)out->mesh.indices.size(),out->mesh.indices); }
    return out;
}
}
