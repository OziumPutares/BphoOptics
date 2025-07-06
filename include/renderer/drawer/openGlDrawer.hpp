#pragma once
#include <GLFW/glfw3.h>
#include <chrono>
#include <renderer/drawer/drawer.hpp>

namespace renderer {
using OpenGLDrawer =
  renderer::drawer::Drawer<void(GLFWwindow const &, std::chrono::nanoseconds)>;

}
