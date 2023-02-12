add_executable( 10_ssao ${CMAKE_CURRENT_SOURCE_DIR}/demo_ssao.cpp)
target_link_libraries( 10_ssao PRIVATE rasterizer_static utility )

target_compile_features( 10_ssao PUBLIC cxx_std_20 )
set_target_properties( 10_ssao PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target( 10_ssao_copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:10_ssao>/assets )
