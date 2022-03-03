add_executable( 05_model ${CMAKE_CURRENT_SOURCE_DIR}/demo_model.cpp)
target_link_libraries( 05_model PRIVATE rasterizer_static utility )

target_compile_features( 05_model PUBLIC cxx_std_20 )
set_target_properties( 05_model PROPERTIES CXX_EXTENSIONS OFF )

add_custom_target( 05_model_copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:05_model>/assets )