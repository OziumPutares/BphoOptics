#include <GLFW/glfw3.h>
#include <array>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>


template<typename T> class Drawer;

template<typename Ret, typename... Params>
class [[nodiscard]] Drawer<Ret(Params...)>
{
  using Signature = Ret(Params...);
  std::function<Signature> m_DrawStrategy;

public:
  template<typename Func>
  constexpr explicit Drawer(Func draw_strategy)
    requires(std::is_invocable_r_v<Ret, Func, Params...>)
    : m_DrawStrategy(draw_strategy)
  {}
  constexpr Drawer(Drawer const &other) = default;
  constexpr Drawer(Drawer &&) = default;
  constexpr Drawer &operator=(Drawer const &other) = default;
  constexpr Drawer &operator=(Drawer &&) = default;
  constexpr ~Drawer() = default;
  // Draw
  auto Draw(Params... args) -> Ret
    requires(!std::same_as<Ret, void>)
  {
    return m_DrawStrategy(args...);
  }
  auto Draw(Params... args) -> Ret
    requires(std::same_as<Ret, void>)
  {
    m_DrawStrategy(args...);
  }
};

template<typename T,
  std::size_t Number,
  template<typename, std::size_t> typename Container = std::array>
class [[nodiscard]] Drawers;

template<typename Ret,
  typename... Params,
  std::size_t Number,
  template<typename, std::size_t> typename Container>
class [[nodiscard]] Drawers<Ret(Params...), Number, Container>
{
  Container<Drawer<Ret(Params...)>, Number> m_Drawers;
};

template<typename T, typename Ret, typename... Args>
concept Drawable = std::same_as<Drawer<Ret(Args...)>, T>;

template<typename... Target>
  requires(!std::same_as<Target, void> && ...)
class DrawTarget
{
  std::atomic<bool> m_Enabled;
  std::tuple<Target...> m_State;


public:
  explicit DrawTarget(Target... draw_target)
    : m_State{ std::move(draw_target)... }
  {}
};
