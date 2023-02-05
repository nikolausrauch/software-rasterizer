add_executable( 05_viewer ${CMAKE_CURRENT_SOURCE_DIR}/demo_viewer.cpp)
target_link_libraries( 05_viewer PRIVATE rasterizer_static utility )

target_compile_features( 05_viewer PUBLIC cxx_std_20 )
set_target_properties( 05_viewer PROPERTIES CXX_EXTENSIONS OFF )
