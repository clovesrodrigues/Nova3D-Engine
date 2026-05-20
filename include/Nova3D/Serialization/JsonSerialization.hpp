#pragma once
#include <Nova3D/Serialization/Serialization.hpp>

namespace nova3d::serialization {

class JsonSerializer final : public Serializer {
public:
    SerializedValue::Object& root() override { return m_root; }
    std::string toJsonString() const;
private:
    SerializedValue::Object m_root;
};

class JsonDeserializer final : public Deserializer {
public:
    explicit JsonDeserializer(SerializedValue::Object root) : m_root(std::move(root)) {}
    static JsonDeserializer fromJsonString(const std::string& json);
    const SerializedValue::Object& root() const override { return m_root; }
private:
    SerializedValue::Object m_root;
};

} // namespace
