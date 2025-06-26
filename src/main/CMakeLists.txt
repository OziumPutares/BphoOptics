add_executable(intro main.cpp)

target_link_libraries(intro PRIVATE myproject::myproject_options myproject::myproject_warnings)
find_program(IWYU_EXECUTABLE NAMES include-what-you-use iwyu)
if(IWYU_EXECUTABLE)
  set(IWYU_COMMAND ${IWYU_EXECUTABLE} --mapping_file=${CMAKE_SOURCE_DIR}/iwyu/glm.imp)

  set_property(TARGET your_target PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_COMMAND})
endif()

target_link_system_libraries(
  intro
  PRIVATE
  fmt::fmt
  spdlog::spdlog
  lefticus::tools
  glfw
  OpenGL::GL
  OpenGL::openGL-Renderer
  glad::glad
  glm::glm)

target_compile_features(intro PUBLIC cxx_std_26)

target_include_directories(intro PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
