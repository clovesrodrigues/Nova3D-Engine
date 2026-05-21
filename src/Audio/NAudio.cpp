#include <Nova3D/Audio/NAudio.hpp>
#include <soloud/soloud.h>
#include <soloud/soloud_wav.h>

namespace nova::audio {
struct NAudioClip::Impl { SoLoud::Wav wav; };
struct NAudioDevice::Impl { SoLoud::Soloud soloud; };
NAudioClip::NAudioClip() : m_impl(std::make_unique<Impl>()) {}
NAudioClip::~NAudioClip() = default;
bool NAudioClip::loadFromFile(const std::string& path) { return m_impl->wav.load(path.c_str()) == SoLoud::SO_NO_ERROR; }
bool NAudioDevice::initialize() { m_impl = std::make_unique<Impl>(); return m_impl->soloud.init() == SoLoud::SO_NO_ERROR; }
void NAudioDevice::shutdown() { if (m_impl) m_impl->soloud.deinit(); m_impl.reset(); }
NAudioVoiceHandle NAudioDevice::play(NAudioClip& clip, float volume, bool loop) { clip.m_impl->wav.setLooping(loop); return NAudioVoiceHandle(m_impl ? m_impl->soloud.play(clip.m_impl->wav, volume) : 0); }
void NAudioDevice::stop(NAudioVoiceHandle handle) { if (m_impl && handle.valid()) m_impl->soloud.stop(handle.m_voice); }
void NAudioDevice::pause(NAudioVoiceHandle handle, bool paused) { if (m_impl && handle.valid()) m_impl->soloud.setPause(handle.m_voice, paused); }
void NAudioDevice::setVolume(NAudioVoiceHandle handle, float volume) { if (m_impl && handle.valid()) m_impl->soloud.setVolume(handle.m_voice, volume); }
}
