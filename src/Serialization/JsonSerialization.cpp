#include <Nova3D/Serialization/JsonSerialization.hpp>
#include <sstream>
namespace nova3d::serialization {
static void writeValue(const SerializedValue& v, std::ostringstream& os){
    std::visit([&](const auto& x){ using T=std::decay_t<decltype(x)>; if constexpr(std::is_same_v<T,std::nullptr_t>) os<<"null"; else if constexpr(std::is_same_v<T,bool>) os<<(x?"true":"false"); else if constexpr(std::is_same_v<T,std::int64_t>||std::is_same_v<T,double>) os<<x; else if constexpr(std::is_same_v<T,std::string>) os<<'"'<<x<<'"'; else if constexpr(std::is_same_v<T,SerializedValue::Array>){ os<<'['; bool f=true; for(auto& i:x){ if(!f) os<<','; f=false; writeValue(i,os);} os<<']'; } else { os<<'{'; bool f=true; for(auto& [k,val]:x){ if(!f) os<<','; f=false; os<<'"'<<k<<"\":"; writeValue(val,os);} os<<'}'; } }, v.value);
}
std::string JsonSerializer::toJsonString() const { std::ostringstream os; SerializedValue v; v.value=m_root; writeValue(v,os); return os.str(); }
JsonDeserializer JsonDeserializer::fromJsonString(const std::string&){ return JsonDeserializer({}); }
}
