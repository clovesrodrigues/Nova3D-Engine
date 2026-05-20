#pragma once
#include <memory>
#include <string>
#include <Nova3D/Animation/Skeleton.hpp>
namespace nova3d::assets { class SkeletonImporter { public: virtual ~SkeletonImporter()=default; virtual std::shared_ptr<animation::Skeleton> importFromFile(const std::string& path)=0; }; }
