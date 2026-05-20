#pragma once
#include <string_view>

namespace nova3d::core {

class Logger {
public:
    static void info(std::string_view message);
    static void warn(std::string_view message);
    static void error(std::string_view message);
};

} // namespace nova3d::core
