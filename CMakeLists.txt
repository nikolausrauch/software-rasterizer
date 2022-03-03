############################################
#           Software Rasterizer            #
#------------------------------------------#
# @author  Nikolaus Rauch                  #
# @date    14.12.2021                      #
#------------------------------------------#
############################################
cmake_minimum_required( VERSION 3.9 )
project( software-rasterizer )

message("\n * CPU Software Rasterizer ")

message(STATUS "${PROJECT_NAME} build environment --")
message(STATUS "Build system:     ${CMAKE_SYSTEM_NAME}-${CMAKE_HOST_SYSTEM_PROCESSOR}")


#########################################
#              CMake-Stuff              #
#########################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)


#########################################
#        Global Compiler Options        #
#########################################
set(GCC_COMPILE_OPTIONS "-Wall")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-ggdb;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")

add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:${GCC_COMPILE_DEBUG_OPTIONS}>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:${GCC_COMPILE_RELEASE_OPTIONS}>")


#########################################
#        Build External-Libraries       #
#########################################
set( OpenGL_GL_PREFERENCE GLVND )
find_package( OpenGL 3.0 REQUIRED )

add_subdirectory(external/glad)
set_property(TARGET glad_static APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
target_include_directories( glad_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include>
    )

add_subdirectory(external/glfw)
set_property(TARGET glfw APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
target_include_directories( glfw PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include>
    )

add_subdirectory(external/stb_image)
set_property(TARGET stb_image APPEND_STRING PROPERTY COMPILE_FLAGS " -w")

add_subdirectory(external/tiny_obj_loader)
set_property(TARGET tiny_obj APPEND_STRING PROPERTY COMPILE_FLAGS " -w")


#########################################
#        Build Software Rasterizer      #
#########################################
add_subdirectory(rasterizer)

#########################################
#           Build Examples              #
#########################################
add_subdirectory(apps)
