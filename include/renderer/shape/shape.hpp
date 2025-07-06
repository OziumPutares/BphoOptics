#pragma once
#include <array>
#include <cstddef>
#include <renderer/colour/colour.hpp>
#include <renderer/point/point.hpp>
#include <renderer/vector/vector.hpp>
#include <strings.h>
namespace renderer {
template<typename T, std::size_t NumberOfSides>
  requires(NumberOfSides > 2)
struct Polygon
{
  std::array<renderer::Point2<T>, NumberOfSides> m_Corners{};
  auto operator[](std::size_t index) noexcept -> renderer::Point2<T> &
  {
    return m_Corners[index];
  }
  auto operator[](std::size_t index) const noexcept
    -> renderer::Point2<T> const &
  {
    return m_Corners[index];
  }
};
template<typename T> using Triangle = Polygon<T, 3>;

}// namespace renderer
