add_executable( 01_indexed_plane ${CMAKE_CURRENT_SOURCE_DIR}/demo_indexed_plane.cpp)
target_link_libraries( 01_indexed_plane PRIVATE rasterizer_static )

target_compile_features( 01_indexed_plane PUBLIC cxx_std_20 )
set_target_properties( 01_indexed_plane PROPERTIES CXX_EXTENSIONS OFF )
