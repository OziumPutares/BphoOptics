#include <GLFW/glfw3.h>
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <renderer/utils/concepts.hpp>
#include <tuple>
#include <type_traits>


namespace renderer::drawer {

template<typename T> class Drawer;

template<typename Ret, typename... Params>
class [[nodiscard]] Drawer<Ret(Params...)>
{
  using Signature = Ret(Params...);
  std::function<Signature> m_DrawStrategy;

public:
  using RetType = Ret;
  using ParamType = std::tuple<Params...>;
  template<typename Func>
  constexpr explicit Drawer(Func draw_strategy)
    requires(std::is_invocable_r_v<RetType, Func, Params...>)
    : m_DrawStrategy(draw_strategy)
  {}
  constexpr Drawer(Drawer const &other) = default;
  constexpr Drawer(Drawer &&) = default;
  constexpr auto operator=(Drawer const &other) -> Drawer & = default;
  constexpr auto operator=(Drawer &&) -> Drawer & = default;
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
  static constexpr std::size_t kNumberOfDrawers = 1 + sizeof...(Drawers);
  using RetType = typename Drawer1<Signature>::RetType;
  using ParamTuple = Drawer1<Signature>::ParamType;
  std::tuple<Drawer1<Signature>, Drawers<Signature>...> m_Drawers;

public:
  explicit StaticDrawerSet(Drawer1<Signature> const &drawer1,
    Drawers<Signature> const &...drawers)
    : m_Drawers(drawer1, drawers...)
  {}
  auto Draw(ParamTuple args) -> void
    requires(std::same_as<RetType, void>)
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
  auto Draw(ParamTuple args) -> std::array<RetType, kNumberOfDrawers>
    requires(!std::same_as<RetType, void>)
  {
    std::array<RetType, kNumberOfDrawers> ReturnValues;
    auto PushBackArray = [Index = 0UZ, &ReturnValues](
                           auto const &value) mutable {
      ReturnValues[Index] = value;
      ++Index;
    };
    auto UseDrawers = [&args, &PushBackArray](auto... drawers) {
      (std::apply([&drawers, &PushBackArray](
                    auto... params) { PushBackArray(drawers.Draw(params...)); },
         args),
        ...);
    };
    std::apply(
      [&UseDrawers](auto... drawers) { return UseDrawers(drawers...); },
      m_Drawers);
    return ReturnValues;
  }
};

template<typename T, typename Ret, typename... Args>
concept Drawable = std::same_as<Drawer<Ret(Args...)>, T>;
template<concepts::signature T, std::size_t Number> class StaticDrawerArray
{
  std::array<Drawer<T>, Number> m_Drawers;

public:
  auto begin() -> decltype(std::begin(m_Drawers))
  {
    return std::begin(m_Drawers);
  }
  auto end() -> decltype(std::end(m_Drawers)) { return std::end(m_Drawers); }
};

}// namespace renderer::drawer
