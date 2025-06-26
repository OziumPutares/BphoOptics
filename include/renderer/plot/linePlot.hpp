#include <GLFW/glfw3.h>
#include <chrono>
#include <iterator>
#include <renderer/vector/vector.hpp>

#include <vector>
// NOLINTNEXTLINE(readability-identifier-naming)
namespace plotting2D {
template<typename T, template<typename> typename Container = std::vector>
  requires requires(Container<T> data) {
    std::begin(data);
    std::end(data);
  }
class Plot
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
  template<typename> typename Container = std::vector>
  requires requires(Container<T> Data) {
    std::begin(Data);
    std::end(Data);
  }
           && std::is_invocable_v<Func,
             GLFWwindow const &,
             renderer::Vector2<float>,
             renderer::Vector2<float>,
             std::chrono::nanoseconds>;

class PlotDrawer : DrawerConcept
{
  Plot<T, Container> m_PlotData;
  renderer::Vector2<float> m_Size;
  renderer::Vector2<float> m_Position;
  Func m_DrawStrategy{};

public:
  void Draw(GLFWwindow const &window,
    std::chrono::nanoseconds delta_time) const override
  {
    m_DrawStrategy(m_Size, m_Position, window, delta_time);
  }
};
}// namespace plotting2D
