add_executable( 11_pbr ${CMAKE_CURRENT_SOURCE_DIR}/demo_pbr.cpp)
target_link_libraries( 11_pbr PRIVATE rasterizer_static utility )

target_compile_features( 11_pbr PUBLIC cxx_std_20 )
set_target_properties( 11_pbr PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target( 11_pbr_copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:11_pbr>/assets )



add_executable( 11_pbr_precompute ${CMAKE_CURRENT_SOURCE_DIR}/pbr_precompute.cpp)
target_link_libraries( 11_pbr_precompute PRIVATE rasterizer_static utility )

target_compile_features( 11_pbr_precompute PUBLIC cxx_std_20 )
set_target_properties( 11_pbr_precompute PROPERTIES CXX_EXTENSIONS OFF )
