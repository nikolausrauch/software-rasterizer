set( UTILITY_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/gl_window.cpp"
    )

set( UTILITY_HDR
    "${CMAKE_CURRENT_SOURCE_DIR}/gl_window.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/detail/shader.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/detail/mesh.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/detail/texture.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/timing.h"

    "${CMAKE_CURRENT_SOURCE_DIR}/model.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/objload.h"
    )

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${UTILITY_SRC} ${UTILITY_HDR} )

add_library( utility STATIC ${UTILITY_SRC} ${UTILITY_HDR} )
target_link_libraries( utility PUBLIC rasterizer_static tiny_obj glfw glad_static OpenGL::GL )
target_include_directories( utility PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    )
