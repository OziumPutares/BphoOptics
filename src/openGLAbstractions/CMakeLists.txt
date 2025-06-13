include(GenerateExportHeader)

add_library(openGL-Renderer shader.cpp error.cpp)

add_library(OpenGL::openGL-Renderer ALIAS openGL-Renderer)

target_link_libraries(
  openGL-Renderer
  PRIVATE myproject_options
          myproject_warnings
          fmt::fmt
          spdlog::spdlog
          lefticus::tools
          glfw
          OpenGL::GL
          glad::glad)

target_include_directories(openGL-Renderer ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                                   $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(openGL-Renderer PUBLIC cxx_std_26)

set_target_properties(
  openGL-Renderer
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(openGL-Renderer EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/renderer/shader/shader.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(openGL-Renderer PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()
