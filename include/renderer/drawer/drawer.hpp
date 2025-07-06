#pragma once
#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <ranges>
#include <renderer/utils/concepts.hpp>
#include <tuple>
#include <type_traits>


namespace renderer::drawer {

template<typename T> class Drawer;

template<typename Ret, typename... Params>
class [[nodiscard]] Drawer<Ret(Params...)>
{
  using Signature = Ret(Params...);
  std::function<Signature> m_DrawStrategy{};

public:
  using RetType = Ret;
  using ParamType = std::tuple<Params...>;
  template<typename Func>
  constexpr explicit Drawer(Func draw_strategy)
    requires(std::is_invocable_r_v<RetType, Func, Params...>)
    : m_DrawStrategy(draw_strategy)
  {}
  Drawer() = default;
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
  std::size_t Number,
  template<typename> typename DrawerType = Drawer>
// requires std::same_as<Drawer<Signature>, DrawerType<Signature>>
class [[nodiscard]] StaticDrawerSet;

template<typename RetType,
  typename... ParamType,
  std::size_t Number,
  template<typename> typename DrawerType>
// requires std::same_as<Drawer<Signature>, DrawerType<Signature>>
class [[nodiscard]] StaticDrawerSet<RetType(ParamType...), Number, DrawerType>
{
  using Signature = RetType(ParamType...);
  std::array<Drawer<Signature>, Number> m_Drawers{};

public:
  constexpr auto begin() noexcept -> decltype(std::begin(m_Drawers))
  {
    return std::begin(m_Drawers);
  }
  constexpr auto begin() const noexcept -> decltype(std::begin(m_Drawers))
  {
    return std::begin(m_Drawers);
  }
  constexpr auto cbegin() const noexcept -> decltype(std::begin(m_Drawers))
  {
    return std::cbegin(m_Drawers);
  }
  constexpr auto end() noexcept -> decltype(std::end(m_Drawers))
  {
    return std::end(m_Drawers);
  }
  constexpr auto end() const noexcept -> decltype(std::end(m_Drawers))
  {
    return std::end(m_Drawers);
  }
  constexpr auto cend() const noexcept -> decltype(std::end(m_Drawers))
  {
    return std::cend(m_Drawers);
  }
  constexpr auto rbegin() noexcept -> decltype(std::rbegin(m_Drawers))
  {
    return std::rbegin(m_Drawers);
  }
  constexpr auto rbegin() const noexcept -> decltype(std::rbegin(m_Drawers))
  {
    return std::rbegin(m_Drawers);
  }
  constexpr auto crbegin() const noexcept -> decltype(std::rbegin(m_Drawers))
  {
    return std::crbegin(m_Drawers);
  }
  constexpr auto rend() noexcept -> decltype(std::rend(m_Drawers))
  {
    return std::rend(m_Drawers);
  }
  constexpr auto rend() const noexcept -> decltype(std::rend(m_Drawers))
  {
    return std::rend(m_Drawers);
  }
  constexpr auto crend() const noexcept -> decltype(std::rend(m_Drawers))
  {
    return std::crend(m_Drawers);
  }
  StaticDrawerSet(std::initializer_list<Drawer<Signature>> list_of_drawers)
  {
    std::ranges::copy(list_of_drawers, std::begin(m_Drawers));
  }
  constexpr auto operator<=>(StaticDrawerSet const &other) const
    -> bool = default;
  constexpr auto Draw(ParamType... params) -> std::array<RetType, Number>
    requires(!std::same_as<concepts::RetType<Signature>, void>)
  {
    std::array<concepts::RetType<Signature>, Number> ReturnValues{};
    for (auto &[index, drawer] : m_Drawers | std::ranges::views::enumerate) {
      ReturnValues[index] = drawer.Draw(params...);
    }
    return ReturnValues;
  }
  constexpr void Draw(ParamType... params)
    requires(std::same_as<RetType, void>)
  {
    std::ranges::for_each(
      m_Drawers, [&params...](auto &value) { value.Draw(params...); });
  }

  constexpr void Draw()
    requires(std::same_as<RetType, void> && std::same_as<void, void>)
  {
    std::ranges::for_each(m_Drawers, [](auto &value) { value.Draw(); });
  }
};
}// namespace renderer::drawer
