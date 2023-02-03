#########################################
#        CPU Software Rasterizer        #
#---------------------------------------#
# @author  Nikolaus Rauch               #
# @date    14.12.2021                   #
#########################################
project( rasterizer )

#################################
#     Viewer Source Files       #
#################################
set( SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/texture.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/framebuffer.cpp"
    )

set( HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/renderer.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/buffer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/program.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/texture.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/sampler.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/framebuffer.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/math/base.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector2.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector3.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/vector4.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix3.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/matrix4.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/math/rectangle.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/detail/test_member.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/detail/tuple_iter.h"
    )

set( RASTERIZER_SRC ${SRC} )
set( RASTERIZER_HDR ${HDR} )

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${VIEWER_SRC} ${VIEWER_HDR} )


#################################
#       Build Viewer Library    #
#################################
add_library( rasterizer_static STATIC ${RASTERIZER_SRC} ${RASTERIZER_HDR} )

target_link_libraries( rasterizer_static PUBLIC stb_image )

target_include_directories( rasterizer_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    )

target_compile_features( rasterizer_static PUBLIC cxx_std_20 )
set_target_properties( rasterizer_static PROPERTIES CXX_EXTENSIONS OFF )
