add_executable( 00_triangle ${CMAKE_CURRENT_SOURCE_DIR}/demo_triangle.cpp)
target_link_libraries( 00_triangle PRIVATE rasterizer_static )

target_compile_features( 00_triangle PUBLIC cxx_std_20 )
set_target_properties( 00_triangle PROPERTIES CXX_EXTENSIONS OFF )
