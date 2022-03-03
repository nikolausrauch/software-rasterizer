set(TINY_OBJ_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.cpp"
	)
	
set(TINY_OBJ_HDR
	"${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.h"
	)
	
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${TINY_OBJ_SRC} ${TINY_OBJ_HDR} )

add_library(tiny_obj STATIC ${TINY_OBJ_SRC} ${TINY_OBJ_HDR} )
target_include_directories( tiny_obj PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    )
    
target_compile_features( tiny_obj PUBLIC cxx_std_17 )
set_target_properties( tiny_obj PROPERTIES CXX_EXTENSIONS OFF )
