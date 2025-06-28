#include <GLFW/glfw3.h>
#include <cassert>
#include <concepts>
#include <functional>
#include <renderer/utils/concepts.hpp>
#include <tuple>
#include <type_traits>


namespace renderer::drawer {

template<typename T> class Drawer;

template<typename... Params> class [[nodiscard]] Drawer<void(Params...)>
{
  using Signature = void(Params...);
  std::function<Signature> m_DrawStrategy;

public:
  using RetType = void;
  using ParamType = std::tuple<Params...>;
  template<typename Func>
  constexpr explicit Drawer(Func draw_strategy)
    requires(std::is_invocable_r_v<RetType, Func, Params...>)
    : m_DrawStrategy(draw_strategy)
  {}
  constexpr Drawer(Drawer const &other) = default;
  constexpr Drawer(Drawer &&) = default;
  constexpr Drawer &operator=(Drawer const &other) = default;
  constexpr Drawer &operator=(Drawer &&) = default;
  constexpr ~Drawer() = default;
  // Draw
  auto Draw(Params... args) -> RetType
    requires(!std::same_as<RetType, void>)
  {
    return m_DrawStrategy(args...);
  }
  auto Draw(Params... args) -> RetType
    requires(std::same_as<RetType, void>)
  {
    m_DrawStrategy(args...);
  }
};

template<concepts::signature Signature,
  template<typename> typename Drawer1,
  template<typename> typename... Drawers>
class [[nodiscard]] StaticDrawerSet
{
  std::tuple<Drawer1<Signature>, Drawers<Signature>...> m_Drawers;

public:
  explicit StaticDrawerSet(Drawer1<Signature> const &drawer1,
    Drawers<Signature> const &...drawers)
    : m_Drawers(drawer1, drawers...)
  {}
  auto Draw(Drawer1<Signature>::ParamType args) -> void
  {
    auto UseDrawers = [&args](auto... drawers) {
      (std::apply(
         [&drawers](auto... params) { drawers.Draw(params...); }, args),
        ...);
    };
    (std::apply(
      [&UseDrawers](auto... drawers) { return UseDrawers(drawers...); },
      m_Drawers));
  }
};

template<typename T, typename Ret, typename... Args>
concept Drawable = std::same_as<Drawer<Ret(Args...)>, T>;

}// namespace renderer::drawer
