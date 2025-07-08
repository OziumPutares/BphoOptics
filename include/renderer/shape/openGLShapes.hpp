#pragma once
#include <glad/glad.h>// NOLINT
//
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstddef>
#include <renderer/drawer/drawer.hpp>
#include <renderer/shader/shader.hpp>
#include <renderer/shape/shape.hpp>
#include <utility>

namespace renderer {
template<typename T, std::size_t NummberOfSides>
inline auto MakePolygonDrawer(Polygon<T, NummberOfSides> triangle,
  gl::Program &&program) -> OpenGLDrawer
{

  auto DrawLambda = [triangle, ProgamCopy = std::move(program)](
                      GLFWwindow const &window,
                      std::chrono::nanoseconds delta_time) {


  };
  return OpenGLDrawer(DrawLambda);
}

}// namespace renderer
