add_executable(intro main.cpp)

target_link_libraries(intro PRIVATE myproject::myproject_options myproject::myproject_warnings)

target_link_system_libraries(
  intro
  PRIVATE
  fmt::fmt
  spdlog::spdlog
  lefticus::tools
  glfw
  OpenGL::GL
  OpenGL::openGL-Renderer
  glad::glad)

target_compile_features(intro PUBLIC cxx_std_26)

target_include_directories(intro PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
