#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
namespace renderer {

struct Red
{
  std::uint8_t Value;
  constexpr static std::size_t kMaxSize =
    std::numeric_limits<decltype(Value)>::max();
};
struct Green
{
  std::uint8_t Value;
  constexpr static std::size_t kMaxSize =
    std::numeric_limits<decltype(Value)>::max();
};
struct Blue
{
  std::uint8_t Value;
  constexpr static std::size_t kMaxSize =
    std::numeric_limits<decltype(Value)>::max();
};
class [[nodiscard]] RedNormalised
{
  float m_Value{};

public:
  RedNormalised() = default;
  constexpr explicit RedNormalised(Red red)
    : m_Value(static_cast<float>(red.Value) / red.kMaxSize)
  {}
};
class [[nodiscard]] GreenNormalised
{
  float m_Value{};

public:
  GreenNormalised() = default;
  constexpr explicit GreenNormalised(Green green)
    : m_Value(static_cast<float>(green.Value) / green.kMaxSize)
  {}
};
struct [[nodiscard]] BlueNormalised
{
  float m_Value{};

public:
  BlueNormalised() = default;
  constexpr explicit BlueNormalised(Blue blue)
    : m_Value(static_cast<float>(blue.Value) / blue.kMaxSize)
  {}
};
class [[nodiscard]] RGBColour
{
  Red m_Red{};
  Green m_Green{};
  Blue m_Blue{};

public:
  constexpr auto Red() noexcept -> Red & { return m_Red; }
  constexpr auto Blue() noexcept -> Blue & { return m_Blue; }
  constexpr auto Green() noexcept -> Green & { return m_Green; }
  [[nodiscard]] constexpr auto Red() const noexcept -> struct Red const &
  {
    return m_Red;
  }
  [[nodiscard]] constexpr auto Blue() const noexcept -> struct Blue const &
  {
    return m_Blue;
  }
  [[nodiscard]] constexpr auto Green() const noexcept -> struct Green const &
  {
    return m_Green;
  }
};
class [[nodiscard]] RBGColourNormalised
{
  RedNormalised m_Red;
  GreenNormalised m_Green;
  BlueNormalised m_Blue;

public:
  constexpr auto Red() noexcept -> RedNormalised & { return m_Red; }
  constexpr auto Blue() noexcept -> BlueNormalised & { return m_Blue; }
  constexpr auto Green() noexcept -> GreenNormalised & { return m_Green; }
  [[nodiscard]] constexpr auto Red() const noexcept -> RedNormalised const &
  {
    return m_Red;
  }
  [[nodiscard]] constexpr auto Blue() const noexcept -> BlueNormalised const &
  {
    return m_Blue;
  }
  [[nodiscard]] constexpr auto Green() const noexcept -> GreenNormalised const &
  {
    return m_Green;
  }
};
}// namespace renderer
