cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

find_package(ignition-rendering0 REQUIRED)

configure_file(config.hh.in build_config.hh @ONLY)

add_executable(example_custom_shaders custom_shaders.cc)

target_include_directories(example_custom_shaders
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${IGNITION-RENDERING_INCLUDE_DIRS})

target_link_libraries(example_custom_shaders
  ${IGNITION-RENDERING_LIBRARIES})