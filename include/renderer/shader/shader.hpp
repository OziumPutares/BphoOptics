#include <glad/glad.h>//
//
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <fmt/core.h>
#include <fmt/format.h>
#include <functional>
#include <renderer/error/error.hpp>
#include <string>
#include <string_view>
#include <utility>
namespace renderer::gl {

template<GLenum Type>
concept IsShaderType = std::ranges::any_of(std::array{ GL_COMPUTE_SHADER,
                                             GL_VERTEX_SHADER,
                                             GL_TESS_CONTROL_SHADER,
                                             GL_TESS_EVALUATION_SHADER,
                                             GL_GEOMETRY_SHADER,
                                             GL_FRAGMENT_SHADER },
  [](GLenum value) { return Type == value; });

template<std::size_t Size, typename Type>
concept ValidUniformSpec =
  (Size <= 4)
  && (std::same_as<Type, unsigned int> || std::same_as<Type, float>
      || std::same_as<Type, int> || std::same_as<Type, double>);

template<GLenum ShaderType>
  requires IsShaderType<ShaderType>
class ShaderUnit
{
  GLuint m_ShaderID{ glCreateShader(ShaderType) };

public:
  [[nodiscard]] constexpr auto GetShaderID() const noexcept -> unsigned int
  {
    return m_ShaderID;
  }
  ShaderUnit(ShaderUnit const &other) = delete;
  ShaderUnit(ShaderUnit &&) = delete;
  auto operator=(ShaderUnit const &) -> ShaderUnit & = delete;
  auto operator=(ShaderUnit &&) -> ShaderUnit & = delete;
  constexpr explicit ShaderUnit(std::string_view source)
    : m_ShaderID{ glCreateShader(ShaderType) }
  {
    // NOLINTNEXTLINE
    auto ShaderSource = reinterpret_cast<GLchar const *>(source.data());
    // Compile shader
    glShaderSource(m_ShaderID, 1, &ShaderSource, nullptr);
    glCompileShader(m_ShaderID);

    // Check if successful
    GLint CompiledSuccessfully{};
    glGetShaderiv(m_ShaderID, GL_COMPILE_STATUS, &CompiledSuccessfully);
    if (CompiledSuccessfully != GL_TRUE) {
      GLsizei LogLength{};
      glGetShaderiv(m_ShaderID, GL_INFO_LOG_LENGTH, &LogLength);
      std::string Message{};
      Message.reserve(static_cast<std::size_t>(LogLength));
      glGetShaderInfoLog(m_ShaderID,
        NULL,
        nullptr,
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<GLchar *>(Message.data()));
      glDeleteShader(m_ShaderID);
      throw renderer::CompilationError(Message);
    }
  }
  constexpr ~ShaderUnit() { glDeleteShader(m_ShaderID); }
};


template<typename T>
concept ShaderType = std::same_as<T, ShaderUnit<GL_COMPUTE_SHADER>>
                     || std::same_as<T, ShaderUnit<GL_VERTEX_SHADER>>
                     || std::same_as<T, ShaderUnit<GL_TESS_CONTROL_SHADER>>
                     || std::same_as<T, ShaderUnit<GL_TESS_EVALUATION_SHADER>>
                     || std::same_as<T, ShaderUnit<GL_GEOMETRY_SHADER>>
                     || std::same_as<T, ShaderUnit<GL_FRAGMENT_SHADER>>;
// Function to add all shader units to program

void AddShaderUnits(unsigned int program_id,
  renderer::gl::ShaderType auto &&first)
{
  glAttachShader(program_id, first.GetShaderID());
}
void AddShaderUnits(unsigned int program_id,
  renderer::gl::ShaderType auto &&first,
  renderer::gl::ShaderType auto &&...shader_units)
{
  glAttachShader(program_id, first.GetShaderID());
  AddShaderUnits(program_id, std::move(shader_units)...);
}
// Main program class
class Program
{
  GLuint m_ProgramID{ glCreateProgram() };


public:
  template<renderer::gl::ShaderType... Args>
  explicit Program(Args... shader_units)
  {
    AddShaderUnits(m_ProgramID, std::move(shader_units)...);
    glLinkProgram(m_ProgramID);
    GLint ProgramLinked{};
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &ProgramLinked);
    if (ProgramLinked != GL_TRUE) {
      GLsizei LogLength{};
      glGetProgramiv(m_ProgramID, GL_INFO_LOG_LENGTH, &LogLength);
      std::string Message{};
      Message.reserve(static_cast<std::size_t>(LogLength));
      glGetProgramInfoLog(m_ProgramID,
        NULL,
        nullptr,
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        reinterpret_cast<GLchar *>(Message.data()));
      glDeleteProgram(m_ProgramID);
      throw renderer::CompilationError(Message);
    }
  }
  void Use() const noexcept { glUseProgram(m_ProgramID); }
  template<std::size_t Size, typename Type>
  void SetUniform(std::string const &name, Type value)
    requires(ValidUniformSpec<Size, Type> && (Size == 1))
  {
    auto Location = glGetUniformLocation(m_ProgramID, name.data());
    if (Location == -1) {
      throw renderer::UniformError(fmt::format("\"{}\"", name));
    }
    if constexpr (std::same_as<Type, float>) {
      glUniform1f(Location, value);
    } else if constexpr (std::same_as<Type, int>) {
      glUniform1i(Location, value);
    } else if constexpr (std::same_as<Type, unsigned int>) {
      glUniform1ui(Location, value);
    } else if constexpr (std::same_as<Type, double>) {
      glUniform1d(Location, value);
    }
  }
  template<std::size_t Size, typename Type>
  void SetUniform(std::string const &name, Type value1, Type value2)
    requires(ValidUniformSpec<Size, Type> && (Size == 2))
  {
    auto Location = glGetUniformLocation(m_ProgramID, name.data());
    if (Location == -1) {
      throw renderer::UniformError(fmt::format("\"{}\"", name));
    }
    if constexpr (std::same_as<Type, float>) {
      glUniform2f(Location, value1, value2);
    } else if constexpr (std::same_as<Type, int>) {
      glUniform2i(Location, value1, value2);
    } else if constexpr (std::same_as<Type, unsigned int>) {
      glUniform2ui(Location, value1, value2);
    } else if constexpr (std::same_as<Type, double>) {
      glUniform2d(Location, value1, value2);
    }
  }
  template<std::size_t Size, typename Type>
  void
    SetUniform(std::string const &name, Type value1, Type value2, Type value3)
    requires(ValidUniformSpec<Size, Type> && (Size == 3))
  {
    auto Location = glGetUniformLocation(m_ProgramID, name.data());
    if (Location == -1) {
      throw renderer::UniformError(fmt::format("\"{}\"", name));
    }
    if constexpr (std::same_as<Type, float>) {
      glUniform3f(Location, value1, value2, value3);
    } else if constexpr (std::same_as<Type, int>) {
      glUniform3i(Location, value1, value2, value3);
    } else if constexpr (std::same_as<Type, unsigned int>) {
      glUniform3ui(Location, value1, value2, value3);
    } else if constexpr (std::same_as<Type, double>) {
      glUniform3d(Location, value1, value2, value3);
    }
  }
  template<std::size_t Size, typename Type>
  void SetUniform(std::string const &name,
    Type value1,
    Type value2,
    Type value3,
    Type value4)
    requires(ValidUniformSpec<Size, Type> && (Size == 4))
  {
    auto Location = glGetUniformLocation(m_ProgramID, name.data());
    if (Location == -1) {
      throw renderer::UniformError(fmt::format("\"{}\" was not found", name));
    }
    if constexpr (std::same_as<Type, float>) {
      glUniform4f(Location, value1, value2, value3, value4);
    } else if constexpr (std::same_as<Type, int>) {
      glUniform4i(Location, value1, value2, value3, value4);
    } else if constexpr (std::same_as<Type, unsigned int>) {
      glUniform4ui(Location, value1, value2, value3, value4);
    } else if constexpr (std::same_as<Type, double>) {
      glUniform4d(Location, value1, value2, value3, value4);
    }
  }
  template<typename Func, typename... Params>
    requires std::invocable<Func, unsigned int, Params...>
  auto UseProgramInFunction(Func &&function, Params... parameters) noexcept(
    noexcept(std::forward<Func>(function)))
  {
    return std::invoke(std::forward<Func>(function),
      m_ProgramID,
      std::forward<Params...>(parameters...));
  }
  Program(Program const &) = delete;
  Program(Program &&) = default;
  auto operator=(Program const &) -> Program & = delete;
  auto operator=(Program &&) -> Program & = default;
  ~Program() { glDeleteProgram(m_ProgramID); }
};
}// namespace renderer::gl
