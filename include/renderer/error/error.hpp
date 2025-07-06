#pragma once
#include <exception>
#include <glad/glad.h>
#include <spdlog/common.h>
#include <string>
#include <string_view>
#include <utility>
namespace renderer {
class CompilationError : std::exception
{
  char const *m_ErrMsg;

public:
  constexpr explicit CompilationError(char const *what) noexcept
    : m_ErrMsg(what)
  {}
  constexpr explicit CompilationError(std::string_view what) noexcept
    : m_ErrMsg(what.data())
  {}
  [[nodiscard]] constexpr auto what() const noexcept -> char const * override
  {
    return m_ErrMsg;
  }
};
class UniformError : std::exception
{
  std::string m_What;

public:
  constexpr explicit UniformError(char const *what) noexcept : m_What(what) {}
  constexpr explicit UniformError(std::string what) noexcept
    : m_What(std::move(what))
  {}
  [[nodiscard]] constexpr auto what() const noexcept -> char const * override
  {
    return m_What.data();
  }
};

auto GetSourceString(GLenum source) -> char const *;
auto GetTypeString(GLenum type) -> char const *;

auto GLEnumErrorSeverityToSpdLog(GLenum severity) -> spdlog::level::level_enum;
}// namespace renderer
