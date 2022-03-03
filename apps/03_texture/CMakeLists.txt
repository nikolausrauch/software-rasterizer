add_executable( 03_texture ${CMAKE_CURRENT_SOURCE_DIR}/demo_texture.cpp)
target_link_libraries( 03_texture PRIVATE rasterizer_static )

target_compile_features( 03_texture PUBLIC cxx_std_20 )
set_target_properties( 03_texture PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target( 03_texture_copy_asset ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:03_texture>/assets )
