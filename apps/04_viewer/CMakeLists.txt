add_executable( 04_viewer ${CMAKE_CURRENT_SOURCE_DIR}/demo_viewer.cpp)
target_link_libraries( 04_viewer PRIVATE rasterizer_static utility )

target_compile_features( 04_viewer PUBLIC cxx_std_20 )
set_target_properties( 04_viewer PROPERTIES CXX_EXTENSIONS OFF )
