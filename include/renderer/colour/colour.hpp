#include <cstdint>
#include <limits>
struct Red
{
  std::uint8_t Value;
};
struct Green
{
  std::uint8_t Value;
};
struct Blue
{
  std::uint8_t Value;
};
class [[nodiscard]] RedNormalised
{
  float m_Value{};

public:
  constexpr explicit RedNormalised(Red red)
    : m_Value(static_cast<float>(red.Value) / std::numeric_limits<std::uint8_t>::max())
  {}
};
struct [[nodiscard]] GreenNormalised
{
  float Value;
};
struct [[nodiscard]] BlueNormalised
{
  float Value;
};
class RGBColour
{
  Red m_Red{};
  Green m_Green{};
  Blue m_Blue{};

public:
  [[nodiscard]] constexpr Red &Red() noexcept { return m_Red; }
  [[nodiscard]] constexpr Blue &Blue() noexcept { return m_Blue; }
  [[nodiscard]] constexpr Green &Green() noexcept { return m_Green; }
  [[nodiscard]] constexpr struct Red const &Red() const noexcept { return m_Red; }
  [[nodiscard]] constexpr struct Blue const &Blue() const noexcept { return m_Blue; }
  [[nodiscard]] constexpr struct Green const &Green() const noexcept { return m_Green; }
};
class RBGColourNormalised
{
  RedNormalised m_Red{};
  GreenNormalised m_Green{};
  BlueNormalised m_Blue{};

public:
  [[nodiscard]] constexpr RedNormalised &Red() noexcept { return m_Red; }
  [[nodiscard]] constexpr BlueNormalised &Blue() noexcept { return m_Blue; }
  [[nodiscard]] constexpr GreenNormalised &Green() noexcept { return m_Green; }
  [[nodiscard]] constexpr RedNormalised const &Red() const noexcept { return m_Red; }
  [[nodiscard]] constexpr BlueNormalised const &Blue() const noexcept { return m_Blue; }
  [[nodiscard]] constexpr GreenNormalised const &Green() const noexcept { return m_Green; }
};
