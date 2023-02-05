add_executable( 08_normal_mapping ${CMAKE_CURRENT_SOURCE_DIR}/demo_normal_mapping.cpp)
target_link_libraries( 08_normal_mapping PRIVATE rasterizer_static utility )

target_compile_features( 08_normal_mapping PUBLIC cxx_std_20 )
set_target_properties( 08_normal_mapping PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target( 08_normal_mapping_copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:08_normal_mapping>/assets )
