add_executable( 09_shadow_mapping ${CMAKE_CURRENT_SOURCE_DIR}/demo_shadow_mapping.cpp)
target_link_libraries( 09_shadow_mapping PRIVATE rasterizer_static utility )

target_compile_features( 09_shadow_mapping PUBLIC cxx_std_20 )
set_target_properties( 09_shadow_mapping PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target( 09_shadow_mapping_copy_asset ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:09_shadow_mapping>/assets )