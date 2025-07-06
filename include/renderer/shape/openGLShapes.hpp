#pragma once
#include <renderer/drawer/openGlDrawer.hpp>
#include <renderer/shape/shape.hpp>

namespace renderer {
template<typename T, typename... Args>
inline auto MakeGenericTriangle(Triangle<T> triangle) -> OpenGLDrawer
{}

}// namespace renderer
