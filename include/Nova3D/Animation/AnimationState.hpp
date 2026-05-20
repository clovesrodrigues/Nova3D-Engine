#pragma once
#include <memory>
#include <vector>
#include <Nova3D/Animation/AnimationClip.hpp>
namespace nova3d::animation {
class AnimationState { public: enum class Status { Stopped, Playing, Paused }; void play(){m_status=Status::Playing;} void pause(){m_status=Status::Paused;} void stop(){m_status=Status::Stopped; m_time=0.0F;} void setSpeed(float speed){m_speed=speed;} float speed() const { return m_speed; } float time() const { return m_time; } void update(float dt, float clipDuration, bool looping); Status status() const { return m_status; } private: Status m_status=Status::Stopped; float m_time=0.0F; float m_speed=1.0F; };
class AnimationPlayer { public: void setClip(std::shared_ptr<AnimationClip> clip){m_clip=std::move(clip);} std::shared_ptr<AnimationClip> clip() const { return m_clip; } AnimationState& state(){return m_state;} const AnimationState& state() const { return m_state; } std::vector<BonePose> sample(const Skeleton& skeleton) const; private: std::shared_ptr<AnimationClip> m_clip; AnimationState m_state; };
class AnimationController { public: void setSkeleton(std::shared_ptr<Skeleton> skeleton){m_skeleton=std::move(skeleton);} void addPlayer(AnimationPlayer player); void update(float dt); const std::vector<AnimationPlayer>& players() const { return m_players; } private: std::shared_ptr<Skeleton> m_skeleton; std::vector<AnimationPlayer> m_players; };
}
