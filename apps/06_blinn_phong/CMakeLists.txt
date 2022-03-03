add_executable( 06_blinn_phong ${CMAKE_CURRENT_SOURCE_DIR}/demo_blinn_phong.cpp)
target_link_libraries( 06_blinn_phong PRIVATE rasterizer_static utility )

target_compile_features( 06_blinn_phong PUBLIC cxx_std_20 )
set_target_properties( 06_blinn_phong PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target( 06_blinn_phong_copy_asset ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:06_blinn_phong>/assets )