add_executable( 04_framebuffer ${CMAKE_CURRENT_SOURCE_DIR}/demo_framebuffer.cpp)
target_link_libraries( 04_framebuffer PRIVATE rasterizer_static )

target_compile_features( 04_framebuffer PUBLIC cxx_std_20 )
set_target_properties( 04_framebuffer PROPERTIES CXX_EXTENSIONS OFF )
