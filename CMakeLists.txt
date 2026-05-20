cmake_minimum_required(VERSION 3.24)
project(Nova3D VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NOVA3D_BUILD_SANDBOX "Build sandbox app" ON)

find_package(SDL2 REQUIRED)

add_library(nova3d
    src/Core/Logger.cpp
    src/Core/Engine.cpp
    src/Platform/PlatformFactory.cpp
    src/Platform/SDL/SDLPlatformServices.cpp
    src/Graphics/OpenGL/OpenGLRenderBackend.cpp
    src/Assets/AssetRegistry.cpp
    src/Resources/ResourceSystem.cpp
    src/IO/FileSystem.cpp
    src/IO/Archives.cpp
    src/Assets/AssetTypes.cpp
    src/Assets/TextureLoader.cpp
    src/Assets/ImporterRegistry.cpp
    src/Scene/Scene.cpp
)

target_include_directories(nova3d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(nova3d PUBLIC SDL2::SDL2)

if(ANDROID)
    find_library(log-lib log)
    target_link_libraries(nova3d PUBLIC GLESv3 EGL ${log-lib})
    target_compile_definitions(nova3d PUBLIC NOVA3D_PLATFORM_ANDROID=1)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(nova3d PUBLIC OpenGL::GL)
    target_compile_definitions(nova3d PUBLIC NOVA3D_PLATFORM_DESKTOP=1)
endif()

if(NOVA3D_BUILD_SANDBOX)
    add_subdirectory(apps/Sandbox)
endif()
