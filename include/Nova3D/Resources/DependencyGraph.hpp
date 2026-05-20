#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace nova3d::resources {
class DependencyGraph { public: void registerDependency(const std::string& owner, const std::string& dependency); std::unordered_set<std::string> dependenciesOf(const std::string& owner) const; private: std::unordered_map<std::string, std::unordered_set<std::string>> m_graph; };
}
