#include <renderer/vector/vector.hpp>
template<typename T> struct Triangle
{
  renderer::Vector2<T> corner1{};
  renderer::Vector2<T> corner2{};
  renderer::Vector2<T> corner3{};
};
