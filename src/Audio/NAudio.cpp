#include <Nova3D/Audio/NAudio.hpp>

#include <Nova3D/Core/Logger.hpp>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

namespace nova::audio {
struct NAudioClip::Impl { SoLoud::Wav wav; bool loaded = false; std::string sourcePath; };
struct NAudioDevice::Impl { SoLoud::Soloud soloud; bool initialized = false; };

NAudioClip::NAudioClip() : m_impl(std::make_unique<Impl>()) {}
NAudioClip::~NAudioClip() = default;

bool NAudioClip::loadFromFile(const std::string& path) {
    const auto err = m_impl->wav.load(path.c_str());
    m_impl->loaded = (err == SoLoud::SO_NO_ERROR);
    m_impl->sourcePath = path;
    if (!m_impl->loaded) {
        nova3d::core::Logger::error("NAudioClip load failed: " + path + " (code=" + std::to_string(err) + ")");
    }
    return m_impl->loaded;
}

bool NAudioDevice::initialize() {
    if (m_impl && m_impl->initialized) return true;
    m_impl = std::make_unique<Impl>();
    const auto err = m_impl->soloud.init();
    m_impl->initialized = (err == SoLoud::SO_NO_ERROR);
    if (!m_impl->initialized) {
        nova3d::core::Logger::error("NAudioDevice init failed (SoLoud error=" + std::to_string(err) + ")");
    }
    return m_impl->initialized;
}

void NAudioDevice::shutdown() {
    if (!m_impl) return;
    if (m_impl->initialized) {
        m_impl->soloud.deinit();
        m_impl->initialized = false;
    }
    m_impl.reset();
}

NAudioVoiceHandle NAudioDevice::play(NAudioClip& clip, float volume, bool loop) {
    if (!m_impl || !m_impl->initialized) {
        nova3d::core::Logger::error("NAudioDevice play ignored: device not initialized");
        return {};
    }
    if (!clip.m_impl || !clip.m_impl->loaded) {
        nova3d::core::Logger::error("NAudioDevice play failed: clip not loaded (" + (clip.m_impl ? clip.m_impl->sourcePath : std::string("null")) + ")");
        return {};
    }
    clip.m_impl->wav.setLooping(loop);
    return NAudioVoiceHandle(m_impl->soloud.play(clip.m_impl->wav, volume));
}

void NAudioDevice::stop(NAudioVoiceHandle handle) { if (m_impl && m_impl->initialized && handle.valid()) m_impl->soloud.stop(handle.m_voice); }
void NAudioDevice::pause(NAudioVoiceHandle handle, bool paused) { if (m_impl && m_impl->initialized && handle.valid()) m_impl->soloud.setPause(handle.m_voice, paused); }
void NAudioDevice::setVolume(NAudioVoiceHandle handle, float volume) { if (m_impl && m_impl->initialized && handle.valid()) m_impl->soloud.setVolume(handle.m_voice, volume); }
}
