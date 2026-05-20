#pragma once
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace nova3d::core {
using ObjectId = std::uint64_t;

class ObjectRegistry {
public:
    template <typename T> ObjectId registerObject(const std::shared_ptr<T>& object){ const auto id=++m_nextId; m_objects[id]=object; return id; }
    std::shared_ptr<void> resolve(ObjectId id) const;
    void unregisterObject(ObjectId id);
private:
    ObjectId m_nextId = 0;
    std::unordered_map<ObjectId, std::weak_ptr<void>> m_objects;
};

} // namespace
