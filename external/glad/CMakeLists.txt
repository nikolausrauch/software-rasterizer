set(GLAD_HDR
	"${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h"
	)
	
set(GLAD_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/src/glad.cpp"
	)
	
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${GLAD_SRC} ${GLAD_HDR} )

add_library(glad_static STATIC ${GLAD_SRC} ${GLAD_HDR} )
target_include_directories( glad_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    )
    
target_compile_features( glad_static PUBLIC cxx_std_17 )
set_target_properties( glad_static PROPERTIES CXX_EXTENSIONS OFF )
