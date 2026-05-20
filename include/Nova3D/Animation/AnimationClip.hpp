#pragma once
#include <string>
#include <vector>
#include <Nova3D/Animation/Skeleton.hpp>
namespace nova3d::animation {
struct Keyframe { float time=0.0F; BoneTransform transform{}; };
struct AnimationChannel { std::int32_t boneIndex=-1; std::vector<Keyframe> keyframes; BoneTransform sample(float time, bool looping, float duration) const; };
struct AnimationTrack { std::string name; std::vector<AnimationChannel> channels; };
class AnimationClip { public: explicit AnimationClip(std::string name=""):m_name(std::move(name)){} const std::string& name() const { return m_name; } void setDuration(float seconds){m_duration=seconds;} float duration() const { return m_duration; } void setLooping(bool looping){m_looping=looping;} bool looping() const { return m_looping; } std::vector<AnimationTrack>& tracks(){return m_tracks;} const std::vector<AnimationTrack>& tracks() const { return m_tracks; } private: std::string m_name; float m_duration=0.0F; bool m_looping=true; std::vector<AnimationTrack> m_tracks; };
}
