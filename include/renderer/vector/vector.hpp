
#include <array>
#include <cstddef>
namespace renderer {

template<typename T, std::size_t Dimension> struct Vector
{
  std::array<T, Dimension> m_Values;
  auto X() noexcept -> T &
    requires(Dimension > 0)
  {
    return m_Values[0];
  };
  auto Y() noexcept -> T &
    requires(Dimension > 1)
  {
    return m_Values[1];
  };
  auto Z() noexcept -> T &
    requires(Dimension > 2)
  {
    return m_Values[2];
  };
  auto W() noexcept -> T &requires(Dimension > 3) { return m_Values[3]; };
};
template<typename T, size_t Num> consteval auto VectorDef()
{
  return Vector<T, Num>{};
}
template<typename T> using Vector1 = decltype(VectorDef<T, 1>());
template<typename T> using Vector2 = decltype(VectorDef<T, 2>());
template<typename T> using Vector3 = decltype(VectorDef<T, 3>());
template<typename T> using Vector4 = decltype(VectorDef<T, 4>());
}// namespace renderer
