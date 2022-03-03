set(STB_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/stb_impl.cpp"
	)
	
set(STB_HDR
	"${CMAKE_CURRENT_SOURCE_DIR}/stb_image_write.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h"
	)
	
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${STB_SRC} ${STB_HDR} )

add_library(stb_image STATIC ${STB_SRC} ${STB_HDR} )
target_include_directories( stb_image PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    )
    
target_compile_features( stb_image PUBLIC cxx_std_20 )
set_target_properties( stb_image PROPERTIES CXX_EXTENSIONS OFF )
