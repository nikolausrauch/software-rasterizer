add_executable( 02_depth_buffer ${CMAKE_CURRENT_SOURCE_DIR}/demo_depth_buffer.cpp)
target_link_libraries( 02_depth_buffer PRIVATE rasterizer_static )

target_compile_features( 02_depth_buffer PUBLIC cxx_std_20 )
set_target_properties( 02_depth_buffer PROPERTIES CXX_EXTENSIONS OFF )
