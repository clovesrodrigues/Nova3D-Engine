#include <algorithm>
#include <Nova3D/Animation/AnimationState.hpp>
namespace nova3d::animation {
BoneTransform AnimationChannel::sample(float time, bool looping, float duration) const { if(keyframes.empty()) return {}; float t=time; if(looping && duration>0){ while(t>duration) t-=duration; } if(t<=keyframes.front().time) return keyframes.front().transform; if(t>=keyframes.back().time) return keyframes.back().transform; for(size_t i=0;i+1<keyframes.size();++i){ const auto& a=keyframes[i]; const auto& b=keyframes[i+1]; if(t>=a.time && t<=b.time){ float alpha=(t-a.time)/std::max(0.0001F,b.time-a.time); return BoneTransform::blendLinear(a.transform,b.transform,alpha);} } return keyframes.back().transform; }
void AnimationState::update(float dt, float clipDuration, bool looping){ if(m_status!=Status::Playing) return; m_time += dt*m_speed; if(looping && clipDuration>0){ while(m_time>clipDuration) m_time-=clipDuration; } else if(m_time>clipDuration){ m_time=clipDuration; m_status=Status::Stopped; }}
std::vector<BonePose> AnimationPlayer::sample(const Skeleton& skeleton) const { auto pose=skeleton.createBindPose(); if(!m_clip) return pose; for(const auto& track:m_clip->tracks()) for(const auto& ch:track.channels) if(ch.boneIndex>=0 && static_cast<size_t>(ch.boneIndex)<pose.size()) pose[static_cast<size_t>(ch.boneIndex)].local=ch.sample(m_state.time(),m_clip->looping(),m_clip->duration()); return pose; }
void AnimationController::addPlayer(AnimationPlayer player){ m_players.push_back(std::move(player)); }
void AnimationController::update(float dt){ for(auto& p:m_players){ if(auto c=p.clip()) p.state().update(dt,c->duration(),c->looping()); } }
}
