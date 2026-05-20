#pragma once
#include <memory>
#include <string>
#include <Nova3D/Animation/AnimationClip.hpp>
namespace nova3d::assets { class AnimationImporter { public: virtual ~AnimationImporter()=default; virtual std::shared_ptr<animation::AnimationClip> importFromFile(const std::string& path)=0; }; }
