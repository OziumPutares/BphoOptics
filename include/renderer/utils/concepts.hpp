
#include <concepts>
#include <tuple>
namespace renderer::concepts {
namespace _impl {
  template<typename T> struct FunctionSignatureInfo
  {
    using ReturnType = void;
    using ParameterTypes = void;
    static constexpr bool kIsValidSignature = false;
  };
  template<typename Ret, typename... Params>
  struct FunctionSignatureInfo<Ret(Params...)>
  {
    using ReturnType = Ret;
    using ParameterTypes = std::tuple<Params...>;
    static constexpr bool kIsValidSignature = true;
  };
}// namespace _impl
template<typename T>
concept signature = _impl::FunctionSignatureInfo<T>::kIsValidSignature;
template<typename T>
concept non_returning_function =
  std::same_as<typename _impl::FunctionSignatureInfo<T>::ReturnType, void>
  && signature<T>;
template<signature T>
using RetType = _impl::FunctionSignatureInfo<T>::ReturnType;
template<signature T>
using ParamTypeTuple = _impl::FunctionSignatureInfo<T>::ParameterTypes;

}// namespace renderer::concepts
