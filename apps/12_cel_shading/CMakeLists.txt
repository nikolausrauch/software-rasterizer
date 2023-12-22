add_executable( 12_cel_shading ${CMAKE_CURRENT_SOURCE_DIR}/demo_cel_shading.cpp)
target_link_libraries( 12_cel_shading PRIVATE rasterizer_static utility )

target_compile_features( 12_cel_shading PUBLIC cxx_std_20 )
set_target_properties( 12_cel_shading PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target( 12_cel_shading_copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:12_cel_shading>/assets )