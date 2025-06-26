#include <cstddef>
#include <cstdint>
#include <fmt/core.h>//NOLINT
#include <iterator>

namespace {

[[nodiscard]] auto SumValues(uint8_t const *data, size_t size)
{
  constexpr auto kScale = 1000;

  int Value = 0;
  for (std::size_t Offset = 0; Offset < size; ++Offset) {
    Value +=
      static_cast<int>(*std::next(data, static_cast<long>(Offset))) * kScale;
  }
  return Value;
}
}// namespace

// Fuzzer that attempts to invoke undefined behavior for signed integer overflow
// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
namespace {
extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
  fmt::print("Value sum: {}, len{}\n", SumValues(data, size), size);
  return 0;
}
}// namespace
