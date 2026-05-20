#include <Nova3D/Core/Logger.hpp>
#include <iostream>

namespace nova3d::core {

void Logger::info(std::string_view message) { std::cout << "[INFO] " << message << '\n'; }
void Logger::warn(std::string_view message) { std::cout << "[WARN] " << message << '\n'; }
void Logger::error(std::string_view message) { std::cerr << "[ERROR] " << message << '\n'; }

} // namespace nova3d::core
