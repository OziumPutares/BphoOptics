#include <renderer/error/error.hpp>
#include <spdlog/common.h>

char const *renderer::GetTypeString(GLenum type)
{
  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    return "Error";
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    return "Deprecated Behavior";
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    return "Undefined Behavior";
  case GL_DEBUG_TYPE_PORTABILITY:
    return "Portability";
  case GL_DEBUG_TYPE_PERFORMANCE:
    return "Performance";
  case GL_DEBUG_TYPE_MARKER:
    return "Marker";
  case GL_DEBUG_TYPE_PUSH_GROUP:
    return "Push Group";
  case GL_DEBUG_TYPE_POP_GROUP:
    return "Pop Group";
  case GL_DEBUG_TYPE_OTHER:
    return "Other";
  default:
    return "Unknown";
  }
}
spdlog::level::level_enum renderer::GLEnumErrorSeverityToSpdLog(GLenum severity)
{
  spdlog::level::level_enum Level = spdlog::level::level_enum::info;
  if (severity == GL_DEBUG_SEVERITY_HIGH) {
    Level = spdlog::level::level_enum::critical;
  } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
    Level = spdlog::level::level_enum::err;
  } else if (severity == GL_DEBUG_SEVERITY_LOW) {
    Level = spdlog::level::level_enum::warn;
  }
  return Level;
}
char const *renderer::GetSourceString(GLenum source)
{
  switch (source) {
  case GL_DEBUG_SOURCE_API:
    return "API";
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    return "Window System";
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    return "Shader Compiler";
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    return "Third Party";
  case GL_DEBUG_SOURCE_APPLICATION:
    return "Application";
  case GL_DEBUG_SOURCE_OTHER:
    return "Other";
  default:
    return "Unknown";
  }
}
