
#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <format>
#include <memory>
#include <optional>
#include <source_location>
#include <stdexcept>
#include <utility>
#include <variant>

constexpr std::size_t kBytesForStackDrawer{ 128 };
struct DrawingConcept
{
  using DrawFuncHolderClass = std::optional<std::unique_ptr<DrawingConcept>>;
  DrawingConcept() = default;
  DrawingConcept(DrawingConcept const &) = default;
  DrawingConcept(DrawingConcept &&) = default;
  DrawingConcept &operator=(DrawingConcept const &) = default;
  DrawingConcept &operator=(DrawingConcept &&) = default;
  virtual void Draw(GLFWwindow const &window, std::chrono::nanoseconds) const = 0;
  [[nodiscard]] constexpr virtual DrawFuncHolderClass Clone(std::optional<std::byte *> = {}) const;
  virtual ~DrawingConcept() = default;
};

template<typename Func>
  requires std::is_invocable_v<Func, GLFWwindow const &, std::chrono::nanoseconds>
class ConcreteDrawer : DrawingConcept
{
  Func m_drawStrategy{};

public:
  explicit ConcreteDrawer(Func function) : m_drawStrategy(std::move(function)) {}
  void Draw(GLFWwindow const &window, std::chrono::nanoseconds delta_time) const override
  {
    m_drawStrategy(window, delta_time);
  }
  [[nodiscard]] constexpr DrawingConcept::DrawFuncHolderClass Clone(
    std::optional<std::byte *> address = {}) const override
  {
    if (address) {
      // NOLINTNEXTLINE
      std::construct_at(reinterpret_cast<DrawingConcept *>(*address), *this);
      return {};
    } else {
      return std::unique_ptr<ConcreteDrawer<Func>>(this);
    }
  }
};
class Drawer
{
  std::variant<std::unique_ptr<DrawingConcept>, std::array<std::byte, kBytesForStackDrawer>> m_concreteDrawHolder;

public:
  Drawer(Drawer const &other)
  {
    if (std::holds_alternative<std::unique_ptr<DrawingConcept>>(m_concreteDrawHolder)) {
      auto Temp = std::get<std::unique_ptr<DrawingConcept>>(other.m_concreteDrawHolder)->Clone();
      if (Temp) {
        m_concreteDrawHolder = std::move(*Temp);
      } else {
        throw std::runtime_error("expected std::unique_ptr but did not receive");
      }
    } else {
      auto Temp =
        // NOLINTNEXTLINE
        reinterpret_cast<DrawingConcept const *>(
          &std::get<std::array<std::byte, kBytesForStackDrawer>>(other.m_concreteDrawHolder))
          ->Clone(std::addressof(std::get<std::array<std::byte, kBytesForStackDrawer>>(m_concreteDrawHolder)[0]));
    }
  }
  Drawer(Drawer &&) = default;
  Drawer &operator=(const Drawer &) {};
  Drawer &operator=(Drawer &&) = default;
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
    if (std::holds_alternative<std::unique_ptr<DrawingConcept>>(m_concreteDrawHolder)) {
      std::get<std::unique_ptr<DrawingConcept>>(m_concreteDrawHolder)->Draw(window, delta_time);
    } else {
      // NOLINTNEXTLINE
      reinterpret_cast<DrawingConcept *>(&std::get<std::array<std::byte, kBytesForStackDrawer>>(m_concreteDrawHolder))
        ->Draw(window, delta_time);
    }
  }
};
