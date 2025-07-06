#pragma once
#include <cstddef>
#include <renderer/colour/colour.hpp>
#include <renderer/vector/vector.hpp>

namespace renderer {
template<typename T, std::size_t Dimension> struct Point
{
  renderer::Vector<T, Dimension> m_PositionVector;
  renderer::RGBColour m_ColourOfPoint;
};
template<typename T> using Point1 = Point<T, 1>;
template<typename T> using Point2 = Point<T, 2>;
template<typename T> using Point3 = Point<T, 3>;
template<typename T> using Point4 = Point<T, 4>;
}// namespace renderer
