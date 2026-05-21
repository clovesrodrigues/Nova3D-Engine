cmake_minimum_required(VERSION 3.24)
project(Nova3D VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NOVA3D_BUILD_SANDBOX "Build sandbox app" ON)
option(NOVA3D_BUILD_VALIDATION "Build validation tools" ON)
option(NOVA3D_REQUIRE_OPENGL "Fail configure when desktop OpenGL dev package is missing" ON)

set(NOVA3D_THIRD_PARTY_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/include")
set(NOVA3D_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(NOVA3D_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_library(nova3d
    src/Core/Logger.cpp src/Core/Engine.cpp src/Platform/PlatformFactory.cpp src/Platform/SDL/SDLPlatformServices.cpp
    src/Graphics/OpenGL/OpenGLRenderBackend.cpp src/Assets/AssetRegistry.cpp src/Resources/ResourceSystem.cpp src/IO/FileSystem.cpp src/IO/Archives.cpp
    src/Assets/AssetTypes.cpp src/Assets/TextureLoader.cpp src/Assets/ImporterRegistry.cpp src/Assets/AssetPipeline.cpp src/Assets/Importers/FBXImporter.cpp
    src/GUI/GUI.cpp src/Fonts/Font.cpp src/Editor/RuntimeUIFoundation.cpp src/Assets/Importers/OBJImporter.cpp src/Graphics/Resources/MeshGPUResource.cpp
    src/Graphics/MaterialInstance.cpp src/Scene/Scene.cpp src/Graphics/RenderPipeline/ForwardRenderer.cpp src/Animation/Skeleton.cpp src/Animation/Animation.cpp
    src/Audio/NAudio.cpp src/Assets/Importers/NModelImporter.cpp src/Terrain/Terrain.cpp src/Spatial/SpatialPartition.cpp src/Serialization/Serialization.cpp
    src/Serialization/JsonSerialization.cpp src/Scene/SceneSerialization.cpp src/Scene/Prefab.cpp src/Reflection/Reflection.cpp src/Core/ObjectRegistry.cpp
    src/Debug/DebugRenderer.cpp src/Editor/RuntimeIntrospection.cpp src/Resources/DependencyGraph.cpp src/Resources/HotReloadHooks.cpp src/Runtime/TaskSystem.cpp
    src/Runtime/RuntimeFoundation.cpp src/Runtime/EngineInfrastructure.cpp src/Memory/MemoryFoundation.cpp)

add_library(Nova3D::SDL2 SHARED IMPORTED GLOBAL)
set_target_properties(Nova3D::SDL2 PROPERTIES IMPORTED_IMPLIB "${NOVA3D_LIB_DIR}/libSDL2.dll.a" IMPORTED_LOCATION "${NOVA3D_BIN_DIR}/SDL2.dll" INTERFACE_INCLUDE_DIRECTORIES "${NOVA3D_THIRD_PARTY_INCLUDE_DIR}/SDL2")
add_library(Nova3D::GLFW STATIC IMPORTED GLOBAL)
set_target_properties(Nova3D::GLFW PROPERTIES IMPORTED_LOCATION "${NOVA3D_LIB_DIR}/libglfw3.a" INTERFACE_INCLUDE_DIRECTORIES "${NOVA3D_THIRD_PARTY_INCLUDE_DIR}/GLFW")
add_library(Nova3D::SoLoud STATIC IMPORTED GLOBAL)
set_target_properties(Nova3D::SoLoud PROPERTIES IMPORTED_LOCATION "${NOVA3D_LIB_DIR}/libsoloud.a" INTERFACE_INCLUDE_DIRECTORIES "${NOVA3D_THIRD_PARTY_INCLUDE_DIR}")
add_library(Nova3D::Assimp SHARED IMPORTED GLOBAL)
set_target_properties(Nova3D::Assimp PROPERTIES IMPORTED_IMPLIB "${NOVA3D_LIB_DIR}/libassimp.dll.a" IMPORTED_LOCATION "${NOVA3D_BIN_DIR}/libassimp-6.dll" INTERFACE_INCLUDE_DIRECTORIES "${NOVA3D_THIRD_PARTY_INCLUDE_DIR}")

target_include_directories(nova3d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(nova3d PUBLIC Nova3D::SDL2 Nova3D::GLFW Nova3D::Assimp Nova3D::SoLoud)

set(NOVA3D_OPENGL_AVAILABLE OFF)
if(ANDROID)
    find_library(log-lib log)
    target_link_libraries(nova3d PUBLIC GLESv3 EGL ${log-lib})
    target_compile_definitions(nova3d PUBLIC NOVA3D_PLATFORM_ANDROID=1)
    set(NOVA3D_OPENGL_AVAILABLE ON)
else()
    find_package(OpenGL QUIET)
    if(OpenGL_FOUND)
        target_link_libraries(nova3d PUBLIC OpenGL::GL)
        target_compile_definitions(nova3d PUBLIC NOVA3D_PLATFORM_DESKTOP=1)
        set(NOVA3D_OPENGL_AVAILABLE ON)
    elseif(NOVA3D_REQUIRE_OPENGL)
        message(FATAL_ERROR "OpenGL development package not found. Install OpenGL headers/libs (e.g. libgl1-mesa-dev) or configure with -DNOVA3D_REQUIRE_OPENGL=OFF to build non-graphics targets only.")
    else()
        message(WARNING "OpenGL not found; graphics executables like Sandbox will be skipped. Core library still configures.")
    endif()
endif()

function(nova3d_copy_runtime_dlls target_name)
    if(WIN32)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${NOVA3D_BIN_DIR}/SDL2.dll" "$<TARGET_FILE_DIR:${target_name}>/SDL2.dll"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${NOVA3D_BIN_DIR}/libassimp-6.dll" "$<TARGET_FILE_DIR:${target_name}>/libassimp-6.dll" VERBATIM)
    endif()
endfunction()

if(NOVA3D_BUILD_SANDBOX AND NOVA3D_OPENGL_AVAILABLE)
    add_subdirectory(apps/Sandbox)
elseif(NOVA3D_BUILD_SANDBOX)
    message(STATUS "Skipping Sandbox target because OpenGL is unavailable.")
endif()

if(NOVA3D_BUILD_VALIDATION)
    add_executable(nova_asset_validator tools/nova_asset_validator.cpp)
endif()
