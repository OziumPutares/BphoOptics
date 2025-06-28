#include <catch2/catch_test_macros.hpp>
// NOLINTNEXTLINE
#include <glad/glad.h>

#include <renderer/drawer/drawer.hpp>
#include <renderer/error/error.hpp>
#include <spdlog/common.h>
#include <string>
#include <string_view>
#include <tuple>
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

TEST_CASE("Drawer works with void return", "[Drawer]")
{
  int CallCount = 0;
  renderer::drawer::Drawer<void()> VoidDrawer([&]() { CallCount++; });
  VoidDrawer.Draw();
  VoidDrawer.Draw();
  REQUIRE((CallCount == 2));
}

TEST_CASE("Drawer copy and move constructors/assignment", "[Drawer]")
{
  int Result = 0;
  renderer::drawer::Drawer<void(int)> Orig([&](int x) { Result = x + 1; });
  renderer::drawer::Drawer<void(int)> Copy = Orig;
  renderer::drawer::Drawer<void(int)> Moved = std::move(Orig);

  // NOLINTNEXTLINE
  Copy.Draw(10);
  REQUIRE((Result == 11));

  // NOLINTNEXTLINE
  Moved.Draw(12);
  REQUIRE((Result == 13));

  renderer::drawer::Drawer<void(int)> Assigned([&](int x) { Result = x - 1; });
  Assigned = Copy;
  // NOLINTNEXTLINE
  Assigned.Draw(20);
  REQUIRE((Result == 21));

  renderer::drawer::Drawer<void(int)> MoveAssigned(
    [&](int x) { Result = x * 2; });
  MoveAssigned = std::move(Copy);
  MoveAssigned.Draw(3);
  REQUIRE((Result == 4));
}
TEST_CASE("StaticDrawerSet works with single drawer", "[StaticDrawerSet]")
{
  int CallCount = 0;
  renderer::drawer::Drawer<void()> SingleDrawer([&]() { CallCount++; });

  renderer::drawer::StaticDrawerSet<void(), renderer::drawer::Drawer> DrawerSet(
    SingleDrawer);

  DrawerSet.Draw(std::make_tuple());
  REQUIRE((CallCount == 1));

  DrawerSet.Draw(std::make_tuple());
  REQUIRE((CallCount == 2));
}

TEST_CASE("renderer::drawer::StaticDrawerSet works with multiple drawers",
  "[StaticDrawerSet]")
{
  int CallCount1 = 0;
  int CallCount2 = 0;
  int CallCount3 = 0;

  renderer::drawer::Drawer<void()> Drawer1([&]() { CallCount1++; });
  renderer::drawer::Drawer<void()> Drawer2([&]() { CallCount2++; });
  renderer::drawer::Drawer<void()> Drawer3([&]() { CallCount3++; });

  renderer::drawer::StaticDrawerSet<void(),
    renderer::drawer::Drawer,
    renderer::drawer::Drawer,
    renderer::drawer::Drawer>
    DrawerSet(Drawer1, Drawer2, Drawer3);

  DrawerSet.Draw(std::make_tuple());

  REQUIRE((CallCount1 == 1));
  REQUIRE((CallCount2 == 1));
  REQUIRE((CallCount3 == 1));

  DrawerSet.Draw(std::make_tuple());

  REQUIRE((CallCount1 == 2));
  REQUIRE((CallCount2 == 2));
  REQUIRE((CallCount3 == 2));
}

TEST_CASE("renderer::drawer::StaticDrawerSet works with parameters",
  "[StaticDrawerSet]")
{
  int Result1 = 0;
  int Result2 = 0;

  renderer::drawer::Drawer<void(int, int)> AddDrawer(
    [&](int x, int y) { Result1 = x + y; });
  renderer::drawer::Drawer<void(int, int)> MultiplyDrawer(
    [&](int x, int y) { Result2 = x * y; });

  renderer::drawer::StaticDrawerSet<void(int, int),
    renderer::drawer::Drawer,
    renderer::drawer::Drawer>
    DrawerSet(AddDrawer, MultiplyDrawer);

  DrawerSet.Draw(std::make_tuple(3, 4));

  REQUIRE((Result1 == 7));// 3 + 4
  REQUIRE((Result2 == 12));// 3 * 4

  // NOLINTNEXTLINE
  DrawerSet.Draw(std::make_tuple(5, 6));

  REQUIRE((Result1 == 11));// 5 + 6
  REQUIRE((Result2 == 30));// 5 * 6
}

TEST_CASE("renderer::drawer::StaticDrawerSet works with single parameter",
  "[StaticDrawerSet]")
{
  int SquareResult = 0;
  int DoubleResult = 0;
  int CubeResult = 0;

  renderer::drawer::Drawer<void(int)> SquareDrawer(
    [&](int x) { SquareResult = x * x; });
  renderer::drawer::Drawer<void(int)> DoubleDrawer(
    [&](int x) { DoubleResult = x * 2; });
  renderer::drawer::Drawer<void(int)> CubeDrawer(
    [&](int x) { CubeResult = x * x * x; });

  renderer::drawer::StaticDrawerSet<void(int),
    renderer::drawer::Drawer,
    renderer::drawer::Drawer,
    renderer::drawer::Drawer>
    DrawerSet(SquareDrawer, DoubleDrawer, CubeDrawer);

  DrawerSet.Draw(3);

  REQUIRE((SquareResult == 9));// 3^2
  REQUIRE((DoubleResult == 6));// 3*2
  REQUIRE((CubeResult == 27));// 3^3

  DrawerSet.Draw(std::make_tuple(4));

  REQUIRE((SquareResult == 16));// 4^2
  REQUIRE((DoubleResult == 8));// 4*2
  REQUIRE((CubeResult == 64));// 4^3
}

TEST_CASE("renderer::drawer::StaticDrawerSet works with mixed operations",
  "[StaticDrawerSet]")
{
  std::string LogOutput;
  int Counter = 0;

  renderer::drawer::Drawer<void(int)> LogDrawer(
    [&](int x) { LogOutput += "Logged: " + std::to_string(x) + " "; });
  renderer::drawer::Drawer<void(int)> CountDrawer([&](int x) { Counter += x; });

  renderer::drawer::StaticDrawerSet<void(int),
    renderer::drawer::Drawer,
    renderer::drawer::Drawer>
    DrawerSet(LogDrawer, CountDrawer);

  // NOLINTNEXTLINE
  DrawerSet.Draw(5);

  REQUIRE((LogOutput == "Logged: 5 "));
  REQUIRE((Counter == 5));

  // NOLINTNEXTLINE
  DrawerSet.Draw(10);

  REQUIRE((LogOutput == "Logged: 5 Logged: 10 "));
  REQUIRE((Counter == 15));
}
