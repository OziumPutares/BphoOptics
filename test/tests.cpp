#include <catch2/catch_test_macros.hpp>
// NOLINTNEXTLINE
#include <glad/glad.h>

#include <renderer/drawer/drawer.hpp>
#include <renderer/error/error.hpp>
#include <spdlog/common.h>
#include <string>
#include <string_view>
#include <utility>
TEST_CASE("Error excceptions", "[std::exception]")
{
  REQUIRE(
    (renderer::CompilationError("Hello").what() == std::string{ "Hello" }));
  REQUIRE((renderer::UniformError("Hi").what() == std::string{ "Hi" }));
}
TEST_CASE("Error enum conversions", "[renderer::GLEnumErrorSeverityToSpdLog]")
{
  REQUIRE((renderer::GLEnumErrorSeverityToSpdLog(GL_HIGH_FLOAT)
           == spdlog::level::info));
  REQUIRE((renderer::GLEnumErrorSeverityToSpdLog(GL_DEBUG_SEVERITY_LOW)
           == spdlog::level::warn));
  REQUIRE((renderer::GLEnumErrorSeverityToSpdLog(GL_DEBUG_SEVERITY_MEDIUM)
           == spdlog::level::err));
  REQUIRE((renderer::GLEnumErrorSeverityToSpdLog(GL_DEBUG_SEVERITY_HIGH)
           == spdlog::level::critical));
}
TEST_CASE("Source enum to str", "[renderer::GetSourceString]")
{
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_API)
           == std::string_view{ "API" }));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_WINDOW_SYSTEM)
           == std::string_view{ "Window System" }));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_SHADER_COMPILER)
           == std::string_view{ "Shader Compiler" }));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_APPLICATION)
           == std::string_view{ "Application" }));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_OTHER)
           == std::string_view{ "Other" }));
  REQUIRE((renderer::GetSourceString(0) == std::string_view{ "Unknown" }));
}
TEST_CASE("Drawer works with int return", "[Drawer]")
{
  // NOLINTNEXTLINE
  Drawer<int(int, int)> AddDrawer([](int a, int b) { return a + b; });
  REQUIRE((AddDrawer.Draw(2, 3) == 5));

  Drawer<int(int)> SquareDrawer([](int x) { return x * x; });
  REQUIRE((SquareDrawer.Draw(4) == 16));
}

TEST_CASE("Drawer works with void return", "[Drawer]")
{
  int CallCount = 0;
  Drawer<void()> VoidDrawer([&]() { CallCount++; });
  VoidDrawer.Draw();
  VoidDrawer.Draw();
  REQUIRE((CallCount == 2));
}

TEST_CASE("Drawer copy and move constructors/assignment", "[Drawer]")
{
  Drawer<int(int)> Orig([](int x) { return x + 1; });
  Drawer<int(int)> Copy = Orig;
  Drawer<int(int)> Moved = std::move(Orig);
  REQUIRE((Copy.Draw(10) == 11));
  REQUIRE((Moved.Draw(12) == 13));

  Drawer<int(int)> Assigned([](int x) { return x - 1; });
  Assigned = Copy;
  REQUIRE((Assigned.Draw(20) == 21));

  Drawer<int(int)> MoveAssigned([](int x) { return x * 2; });
  MoveAssigned = std::move(Copy);
  REQUIRE((MoveAssigned.Draw(3) == 4));
}
