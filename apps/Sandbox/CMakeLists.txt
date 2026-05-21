add_executable(nova_sandbox main.cpp)
target_link_libraries(nova_sandbox PRIVATE nova3d)
nova3d_copy_runtime_dlls(nova_sandbox)
