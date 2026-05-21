#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace nova::audio {

class NAudioClip {
public:
    NAudioClip();
    ~NAudioClip();
    NAudioClip(const NAudioClip&) = delete;
    NAudioClip& operator=(const NAudioClip&) = delete;
    bool loadFromFile(const std::string& path);
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    friend class NAudioDevice;
};

class NAudioVoiceHandle {
public:
    NAudioVoiceHandle() = default;
    explicit NAudioVoiceHandle(std::uint32_t v) : m_voice(v) {}
    bool valid() const { return m_voice != 0; }
private:
    std::uint32_t m_voice = 0;
    friend class NAudioDevice;
};

class NAudioDevice {
public:
    bool initialize();
    void shutdown();
    NAudioVoiceHandle play(NAudioClip& clip, float volume, bool loop);
    void stop(NAudioVoiceHandle handle);
    void pause(NAudioVoiceHandle handle, bool paused);
    void setVolume(NAudioVoiceHandle handle, float volume);
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace nova::audio
