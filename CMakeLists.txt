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
set(GCC_COMPILE_OPTIONS "-W;-Wall;-Wno-unused-variable;-Wno-unused-parameter")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-ggdb;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")
set(GCC_COMPILE_SUPPRESS_WARING "-w")

set(MSVC_COMPILE_OPTIONS "/W3;/wd4244;/wd4305")
set(MSVC_COMPILE_DEBUG_OPTIONS "${MSVC_COMPILE_OPTIONS};/Zi")
set(MSVC_COMPILE_RELEASE_OPTIONS "${MSVC_COMPILE_OPTIONS};/O2")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:${GCC_COMPILE_DEBUG_OPTIONS}>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:${GCC_COMPILE_RELEASE_OPTIONS}>")

add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:DEBUG>>:${MSVC_COMPILE_DEBUG_OPTIONS}>")
add_compile_options("$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:RELEASE>>:${MSVC_COMPILE_RELEASE_OPTIONS}>")


#########################################
#        Build External-Libraries       #
#########################################
set( OpenGL_GL_PREFERENCE GLVND )
find_package( OpenGL 3.0 REQUIRED )

add_subdirectory(external/glad)
target_compile_options(glad_static PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")
target_include_directories( glad_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include>
    )

add_subdirectory(external/glfw)
target_compile_options(glfw PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")
target_include_directories( glfw PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include>
    )

add_subdirectory(external/stb_image)
target_compile_options(stb_image PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")

add_subdirectory(external/tiny_obj_loader)
target_compile_options(tiny_obj PRIVATE "$<$<CXX_COMPILER_ID:GNU>:${GCC_COMPILE_SUPPRESS_WARING}>")


#########################################
#        Build Software Rasterizer      #
#########################################
add_subdirectory(rasterizer)

#########################################
#           Build Examples              #
#########################################
add_subdirectory(apps)
