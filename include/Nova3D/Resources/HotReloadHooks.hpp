#pragma once
#include <functional>
#include <string>
#include <vector>

namespace nova3d::resources {
class HotReloadHooks { public: using Callback=std::function<void(const std::string&)>; void onInvalidate(Callback cb){m_invalidate.push_back(std::move(cb));} void invalidate(const std::string& id); private: std::vector<Callback> m_invalidate;};
}
