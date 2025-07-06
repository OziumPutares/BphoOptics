#pragma once
#include <GLFW/glfw3.h>
#include <iterator>
#include <ranges>
#include <renderer/utils/concepts.hpp>
#include <renderer/vector/vector.hpp>

#include <vector>
// NOLINTNEXTLINE(readability-identifier-naming)
namespace LinePlots {
template<typename T, template<typename> typename Container = std::vector>
  requires requires(Container<T> data) {
    std::begin(data);
    std::end(data);
  }
class Plot2d
{
  Container<T> m_Data;

public:
  // NOLINTNEXTLINE(readability-identifier-naming)
  auto begin() -> decltype(auto) { return std::begin(m_Data); }
  // NOLINTNEXTLINE(readability-identifier-naming)
  auto end() -> decltype(auto) { return std::end(m_Data); }
};
template<typename T,
  typename Func,
  template<typename> typename Container = std::vector,
  renderer::concepts::signature Signature>
  requires(std::ranges::range<Container<T>>)
class Plot2DDrawer
{
  Plot2d<T, Container> m_PlotData;
  renderer::Vector2<float> m_Size{};
  renderer::Vector2<float> m_Position{};
  Func m_DrawStrategy{};

public:
  void Draw() const { m_DrawStrategy(m_Size, m_Position, window, delta_time); }
};
}// namespace LinePlots
