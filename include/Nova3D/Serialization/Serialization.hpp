#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

namespace nova3d::serialization {

struct AssetReference { std::string type; std::string id; };

struct SerializedValue {
    using Array = std::vector<SerializedValue>;
    using Object = std::unordered_map<std::string, SerializedValue>;
    using Variant = std::variant<std::nullptr_t, bool, std::int64_t, double, std::string, Array, Object>;
    Variant value{};
};

class SerializationContext { public: std::uint32_t version = 1; bool deltaMode = false; bool networkReplicationMode = false; };

class Serializer { public: virtual ~Serializer() = default; virtual SerializedValue::Object& root() = 0; };
class Deserializer { public: virtual ~Deserializer() = default; virtual const SerializedValue::Object& root() const = 0; };

class SerializableObject {
public:
    virtual ~SerializableObject() = default;
    virtual void serialize(Serializer& serializer, SerializationContext& context) const = 0;
    virtual void deserialize(const Deserializer& deserializer, const SerializationContext& context) = 0;
};

} // namespace nova3d::serialization
