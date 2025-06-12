#include <catch2/catch_test_macros.hpp>

#include <renderer/error/error.hpp>
#include <spdlog/common.h>
#include <string>
#include <string_view>
TEST_CASE("Error handlers", "[std::exception]")
{
  REQUIRE((renderer::CompilationError("Hello").what() == std::string{ "Hello" }));
  REQUIRE((renderer::UniformError("Hi").what() == std::string{ "Hi" }));
  REQUIRE((renderer::GLEnumErrorSeverityToSpdLog(GL_DEBUG_SEVERITY_LOW) == spdlog::level::warn));
  REQUIRE((renderer::GLEnumErrorSeverityToSpdLog(GL_DEBUG_SEVERITY_MEDIUM) == spdlog::level::err));
  REQUIRE((renderer::GLEnumErrorSeverityToSpdLog(GL_DEBUG_SEVERITY_HIGH) == spdlog::level::critical));
  REQUIRE_THROWS((renderer::GLEnumErrorSeverityToSpdLog(GL_HIGH_FLOAT)));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_API) == std::string_view{ "API" }));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_WINDOW_SYSTEM) == std::string_view{ "Window System" }));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_SHADER_COMPILER) == std::string_view{ "Shader Compiler" }));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_APPLICATION) == std::string_view{ "Application" }));
  REQUIRE((renderer::GetSourceString(GL_DEBUG_SOURCE_OTHER) == std::string_view{ "Other" }));
  REQUIRE((renderer::GetSourceString(0) == std::string_view{ "Unknown" }));
}
