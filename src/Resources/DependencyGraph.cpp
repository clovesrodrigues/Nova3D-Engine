#include <Nova3D/Resources/DependencyGraph.hpp>
namespace nova3d::resources {
void DependencyGraph::registerDependency(const std::string& owner, const std::string& dependency){ m_graph[owner].insert(dependency);} 
std::unordered_set<std::string> DependencyGraph::dependenciesOf(const std::string& owner) const { if(auto it=m_graph.find(owner); it!=m_graph.end()) return it->second; return {}; }
}
