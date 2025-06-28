#include <glad/glad.h>//
//
#include <GLFW/glfw3.h>
#include <array>
#include <exception>
#include <filesystem>
#include <fstream>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <renderer/drawer/drawer.hpp>
#include <renderer/shader/shader.hpp>
#include <renderer/vector/vector.hpp>
#include <sstream>
#include <string>
namespace {
auto ReadFile(std::filesystem::path const &location) -> std::string
{
  std::stringstream TempBuffer;
  TempBuffer << std::ifstream(location).rdbuf();
  return TempBuffer.str();
}
// NOLINTBEGIN
std::string ReadFile(auto...) = delete("No Implicit conversions allowed");
void GLAPIENTRY DebugCallback(GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  [[maybe_unused]] GLsizei length,
  GLchar const *message,
  [[maybe_unused]] void const *userParam)
{
  // Ignore non-significant or ignored IDs
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

  spdlog::log(renderer::GLEnumErrorSeverityToSpdLog(severity),
    "OpenGL Debug Message: Source: {}; Type: {}; Message: "
    "{}",
    renderer::GetSourceString(source),
    renderer::GetTypeString(type),
    message);
}
// NOLINTEND

// Window dimensions

constexpr int kXStartingWidth = 800;
constexpr int kYStartingWidth = 600;
// NOLINTBEGIN
int WindowWidth = kXStartingWidth;
int WindowHeight = kYStartingWidth;
// NOLINTEND

// Resize callback function
void FramebufferSizeCallback(GLFWwindow * /*window*/, int width, int height)
{
  WindowWidth = width;
  WindowHeight = height;
  glViewport(0, 0, width, height);
}


}// namespace

// NOLINTNEXTLINE
int main()
{
  try {
    // Initialize GLFW
    if (glfwInit() == 0) {
      std::cerr << "Failed to initialize GLFW\n";
      return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);// NOLINT
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Create window
    GLFWwindow *Window = glfwCreateWindow(
      WindowWidth, WindowHeight, "Resizable OpenGL Window", nullptr, nullptr);
    if (Window == nullptr) {
      std::cerr << "Failed to create GLFW window\n";
      glfwTerminate();
      return -1;
    }

    // Make context current
    glfwMakeContextCurrent(Window);

    // Set the resize callback - THIS IS THE KEY PART
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

    // Initialize GLAD
    if (gladLoadGL() == 0) {
      std::cerr << "Failed to initialize GLAD\n";
      return -1;
    }

    glDebugMessageCallback(DebugCallback, nullptr);
    // NOLINTNEXTLINE
    auto Positions = std::array{
      renderer::Vector3<float>{ { -1.0F, -1.0F, 0.0F } },
      renderer::Vector3<float>{ { 1.0F, 0.0F, 0.0F } },
      renderer::Vector3<float>{ { 0.0F, -1.0F, 0.0F } },
      renderer::Vector3<float>{ { 0.0F, 1.0F, 0.0F } },
      // NOLINTNEXTLINE
      renderer::Vector3<float>{ { 0.0F, -0.5F, 0.0F } },
      renderer::Vector3<float>{ { 0.0F, 0.0F, 1.0F } },
    };
    std::cout << Positions[0].X();
    unsigned int buffer;// NOLINT
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
      std::size(Positions) * sizeof(renderer::Vector2<float>),
      &Positions,
      GL_DYNAMIC_DRAW);
    glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(renderer::Vector3<float>) * 2, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(renderer::Vector3<float>) * 2,
      // NOLINTNEXTLINE
      reinterpret_cast<void *>(sizeof(renderer::Vector3<float>)));
    glEnableVertexAttribArray(1);
    auto VertexShaderUnitMaker = []() {
      return renderer::gl::ShaderUnit<GL_VERTEX_SHADER>(
        ReadFile(std::filesystem::current_path() / "glsl"
                 / "newBaseVertexShader.vert.glsl"));
    };
    auto FragmentShaderUnitMaker = []() {
      return renderer::gl::ShaderUnit<GL_FRAGMENT_SHADER>(
        ReadFile(std::filesystem::current_path() / "glsl"
                 / "ourColourFragmentShader.frag.glsl"));
    };
    auto Program = renderer::gl::Program{ VertexShaderUnitMaker(),
      FragmentShaderUnitMaker() };
    Program.Use();


    // Set initial viewport
    glViewport(0, 0, WindowWidth, WindowHeight);

    using OpenGLDrawer = renderer::drawer::Drawer<void(
      GLFWwindow const &, std::chrono::nanoseconds)>;
    OpenGLDrawer ClearDrawer(
      []([[maybe_unused]] GLFWwindow const &window,
        [[maybe_unused]] std::chrono::nanoseconds delta_time) {
        // NOLINTNEXTLINE
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
      });

    OpenGLDrawer TriangleDrawer(
      [&Program, &VAO](GLFWwindow const & /*window*/,
        [[maybe_unused]] std::chrono::nanoseconds delta_time) -> void {
        Program.Use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
      });
    auto PreviousTime = std::chrono::system_clock::now();
    // Main loop
    while (glfwWindowShouldClose(Window) == 0) {
      auto const StartTime = std::chrono::system_clock::now();
      PreviousTime = StartTime;
      std::chrono::nanoseconds const DeltaTime = StartTime - PreviousTime;
      // Clear screen
      ClearDrawer.Draw(*Window, DeltaTime);

      //
      // RENDER
      //
      TriangleDrawer.Draw(*Window, DeltaTime);

      // Swap buffers and poll events
      glfwSwapBuffers(Window);
      glfwPollEvents();
    }
    glDeleteBuffers(1, &buffer);
    glDeleteVertexArrays(1, &VAO);

    // Clean up
    glfwDestroyWindow(Window);
    glfwTerminate();
    return 0;
  } catch (std::exception const &Err) {
    spdlog::error("{}", Err.what());
  }
}
