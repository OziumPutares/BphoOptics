#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <utility>
#include <variant>

constexpr std::size_t kBytesForStackDrawer{ 128 };
struct DrawerConcept
{
  using DrawFuncHolderClass = std::optional<std::unique_ptr<DrawerConcept>>;
  DrawerConcept() = default;
  DrawerConcept(DrawerConcept const &) = default;
  DrawerConcept(DrawerConcept &&) = default;
  DrawerConcept &operator=(DrawerConcept const &) = default;
  DrawerConcept &operator=(DrawerConcept &&) = default;
  virtual void Draw(GLFWwindow const &window, std::chrono::nanoseconds) const = 0;
  virtual ~DrawerConcept() = default;
};

template<typename Func>
  requires std::is_invocable_v<Func, GLFWwindow const &, std::chrono::nanoseconds>
class ConcreteDrawer : DrawerConcept
{

  Func m_drawStrategy{};

public:
  explicit ConcreteDrawer(Func function) : m_drawStrategy(std::move(function)) {}
  void Draw(GLFWwindow const &window, std::chrono::nanoseconds delta_time) const override
  {
    m_drawStrategy(window, delta_time);
  }
};
class Drawer
{
  std::variant<std::unique_ptr<DrawerConcept>, std::array<std::byte, kBytesForStackDrawer>> m_concreteDrawHolder;

public:
  Drawer(Drawer const &other) = delete;
  Drawer(Drawer &&) = default;
  Drawer &operator=(const Drawer &other) = delete;
  Drawer &operator=(Drawer &&) = default;
  ~Drawer() = default;
  template<typename Func>
    requires std::invocable<Func, GLFWwindow const &, std::chrono::nanoseconds>
  explicit Drawer(Func drawing_function)
  {
    // NOLINTNEXTLINE
    if constexpr (sizeof(ConcreteDrawer<Func>) <= kBytesForStackDrawer) {
      m_concreteDrawHolder = std::array<std::byte, kBytesForStackDrawer>{};
      std::construct_at(
        // NOLINTNEXTLINE
        reinterpret_cast<ConcreteDrawer<Func> *>(
          std::addressof(std::get<std::array<std::byte, kBytesForStackDrawer>>(m_concreteDrawHolder))),
        // NOLINTNEXTLINE
        ConcreteDrawer<Func>(drawing_function));
    } else {
      m_concreteDrawHolder.emplace(std::unique_ptr(ConcreteDrawer(drawing_function)));
    }
  }
  // NOLINTNEXTLINE
  void Draw(GLFWwindow const &window, std::chrono::nanoseconds delta_time)
  {
    // NOLINTNEXTLINE
    if (std::holds_alternative<std::unique_ptr<DrawerConcept>>(m_concreteDrawHolder)) {
      std::get<std::unique_ptr<DrawerConcept>>(m_concreteDrawHolder)->Draw(window, delta_time);
    } else {
      // NOLINTNEXTLINE
      reinterpret_cast<DrawerConcept *>(&std::get<std::array<std::byte, kBytesForStackDrawer>>(m_concreteDrawHolder))
        ->Draw(window, delta_time);
    }
  }
};
