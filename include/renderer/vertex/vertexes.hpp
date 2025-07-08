#pragma once
#include <glad/glad.h>//
//
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <ranges>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>

namespace gl {

// OpenGL error types
struct BufferCreationError
{
  [[nodiscard]] static consteval auto message() noexcept -> std::string_view
  {
    return "Failed to create OpenGL buffer";
  }
};

struct VertexArrayCreationError
{
  [[nodiscard]] static consteval auto message() noexcept -> std::string_view
  {
    return "Failed to create OpenGL vertex array";
  }
};

struct InvalidOperationError
{
  [[nodiscard]] static consteval auto message() noexcept -> std::string_view
  {
    return "Invalid OpenGL operation";
  }
};

// OpenGL object concepts
template<typename T>
concept opengl_object = requires(T t_value) {
  { t_value.get_id() } -> std::same_as<GLuint>;
  { t_value.is_valid() } -> std::same_as<bool>;
  t_value.bind();
  t_value.unbind();
};

template<typename T>
concept vertex_data =
  std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>
  && !std::is_same_v<T, std::byte>;

template<typename T>
concept numeric_type = std::is_arithmetic_v<T>;

// GL type mapping
template<numeric_type T>
constexpr GLenum kGlTypeV = []() constexpr {
  // NOLINTNEXTLINE
  if constexpr (std::is_same_v<T, float>) {
    return GL_FLOAT;
  } else if constexpr (std::is_same_v<T, double>) {
    return GL_DOUBLE;
  } else if constexpr (std::is_same_v<T, std::int8_t>) {
    return GL_BYTE;
  } else if constexpr (std::is_same_v<T, std::uint8_t>) {
    return GL_UNSIGNED_BYTE;
  } else if constexpr (std::is_same_v<T, std::int16_t>) {
    return GL_SHORT;
  } else if constexpr (std::is_same_v<T, std::uint16_t>) {
    return GL_UNSIGNED_SHORT;
  } else if constexpr (std::is_same_v<T, std::int32_t>) {
    return GL_INT;
  } else if constexpr (std::is_same_v<T, std::uint32_t>) {
    return GL_UNSIGNED_INT;
  } else {
    static_assert(false, "Unsupported type for OpenGL");
  }
}();

// Constexpr utility functions
template<numeric_type T> constexpr auto GlTypeSize() noexcept -> std::size_t
{
  return sizeof(T);
}

template<numeric_type T>
constexpr auto GlTypeAlignment() noexcept -> std::size_t
{
  return alignof(T);
}

template<vertex_data T> constexpr auto VertexSize() noexcept -> std::size_t
{
  return sizeof(T);
}

template<vertex_data T> constexpr auto VertexAlignment() noexcept -> std::size_t
{
  return alignof(T);
}

// Constexpr offset calculation
template<vertex_data VertexType, typename MemberType>
// NOLINTNEXTLINE
constexpr auto MemberOffset(MemberType VertexType::*member) noexcept
  -> std::size_t
{
  return offsetof(VertexType, member);
}

// Generic OpenGL object handle
template<auto DeleterFunc> class GlHandle
{
private:
  GLuint m_id{ 0 };

public:
  constexpr explicit GlHandle(GLuint identifier) noexcept : m_id{ identifier }
  {}

  ~GlHandle()
  {
    if (m_id != 0) { DeleterFunc(1, &m_id); }
  }

  GlHandle(GlHandle const &) = delete;
  auto operator=(GlHandle const &) -> GlHandle & = delete;

  constexpr GlHandle(GlHandle &&other) noexcept
    : m_id{ std::exchange(other.m_id, 0) }
  {}

  constexpr auto operator=(GlHandle &&other) noexcept -> GlHandle &
  {
    if (this != &other) {
      if (m_id != 0) { DeleterFunc(1, &m_id); }
      m_id = std::exchange(other.m_id, 0);
    }
    return *this;
  }

  [[nodiscard]] constexpr auto get_id() const noexcept -> GLuint
  {
    return m_id;
  }
  [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
  {
    return m_id != 0;
  }

  constexpr explicit operator bool() const noexcept { return is_valid(); }
};

// Buffer handle
class BufferHandle
{
private:
  GLuint m_id{ 0 };

public:
  constexpr explicit BufferHandle(GLuint identifier) noexcept
    : m_id{ identifier }
  {}

  ~BufferHandle()
  {
    if (m_id != 0) { glDeleteBuffers(1, &m_id); }
  }

  BufferHandle(BufferHandle const &) = delete;
  auto operator=(BufferHandle const &) -> BufferHandle & = delete;

  constexpr BufferHandle(BufferHandle &&other) noexcept
    : m_id{ std::exchange(other.m_id, 0) }
  {}

  constexpr auto operator=(BufferHandle &&other) noexcept -> BufferHandle &
  {
    if (this != &other) {
      if (m_id != 0) { glDeleteBuffers(1, &m_id); }
      m_id = std::exchange(other.m_id, 0);
    }
    return *this;
  }

  [[nodiscard]] constexpr auto get_id() const noexcept -> GLuint
  {
    return m_id;
  }
  [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
  {
    return m_id != 0;
  }

  constexpr explicit operator bool() const noexcept { return is_valid(); }
};

class VertexArrayHandle
{
private:
  GLuint m_id{ 0 };

public:
  constexpr VertexArrayHandle() = default;
  constexpr explicit VertexArrayHandle(GLuint identifier) noexcept
    : m_id{ identifier }
  {}

  ~VertexArrayHandle()
  {
    if (m_id != 0) { glDeleteVertexArrays(1, &m_id); }
  }

  VertexArrayHandle(VertexArrayHandle const &) = delete;
  auto operator=(VertexArrayHandle const &) -> VertexArrayHandle & = delete;

  constexpr VertexArrayHandle(VertexArrayHandle &&other) noexcept
    : m_id{ std::exchange(other.m_id, 0) }
  {}

  constexpr auto operator=(VertexArrayHandle &&other) noexcept
    -> VertexArrayHandle &
  {
    if (this != &other) {
      if (m_id != 0) { glDeleteVertexArrays(1, &m_id); }
      m_id = std::exchange(other.m_id, 0);
    }
    return *this;
  }

  [[nodiscard]] constexpr auto get_id() const noexcept -> GLuint
  {
    return m_id;
  }
  [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
  {
    return m_id != 0;
  }

  constexpr explicit operator bool() const noexcept { return is_valid(); }
};


// Vertex buffer
class VertexBuffer
{
private:
  BufferHandle m_handle;
  GLenum m_target;

public:
  [[nodiscard]] static auto create(
    [[maybe_unused]] GLenum target = GL_ARRAY_BUFFER) noexcept
    -> std::expected<VertexBuffer, BufferCreationError>
  {
    GLuint Identifier{};
    glGenBuffers(1, &Identifier);

    if (Identifier == 0) { return std::unexpected{ BufferCreationError{} }; }

    return VertexBuffer{ BufferHandle{ Identifier }, target };
  }

  void bind() const noexcept { glBindBuffer(m_target, m_handle.get_id()); }

  void unbind() const noexcept { glBindBuffer(m_target, 0); }

  template<vertex_data T>
  void set_data(std::span<T const> data,
    GLenum usage = GL_STATIC_DRAW) const noexcept
  {
    bind();
    glBufferData(m_target, data.size_bytes(), data.data(), usage);
  }

  template<std::ranges::contiguous_range R>
    requires vertex_data<std::ranges::range_value_t<R>>
  void set_data(R const &range, GLenum usage = GL_STATIC_DRAW) const noexcept
  {
    set_data(std::span{ range }, usage);
  }

  template<vertex_data T>
  void set_sub_data(std::span<T const> data, size_t offset_bytes) const noexcept
  {
    bind();
    glBufferSubData(m_target, offset_bytes, data.size_bytes(), data.data());
  }

  template<vertex_data T>
  void set_sub_data(std::span<T const> data,
    size_t offset_elements) const noexcept
    requires std::same_as<T, std::ranges::range_value_t<decltype(data)>>
  {
    set_sub_data(data, offset_elements * sizeof(T));
  }

  // Constexpr utilities for compile-time calculations
  template<vertex_data T>
  static constexpr auto calculate_buffer_size(
    std::size_t element_count) noexcept -> std::size_t
  {
    return element_count * sizeof(T);
  }

  template<vertex_data T>
  static constexpr auto calculate_element_offset(
    std::size_t element_index) noexcept -> std::size_t
  {
    return element_index * sizeof(T);
  }

  [[nodiscard]] constexpr auto get_id() const noexcept -> GLuint
  {
    return m_handle.get_id();
  }
  [[nodiscard]] constexpr auto get_target() const noexcept -> GLenum
  {
    return m_target;
  }
  [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
  {
    return m_handle.is_valid();
  }

private:
  // NOLINTNEXTLINE
  constexpr explicit VertexBuffer(BufferHandle &&handle, GLenum target) noexcept
    : m_handle{ std::move(handle) }, m_target{ target }
  {}
};

// Vertex array
class VertexArray
{
private:
  VertexArrayHandle m_handle;

public:
  [[nodiscard]] static auto create() noexcept
    -> std::expected<VertexArray, VertexArrayCreationError>
  {
    GLuint Identifier{};
    glGenVertexArrays(1, &Identifier);

    if (Identifier == 0) {
      return std::unexpected{ VertexArrayCreationError{} };
    }

    return VertexArray{ VertexArrayHandle{ Identifier } };
  }

  void bind() const noexcept { glBindVertexArray(m_handle.get_id()); }

  static void unbind() noexcept { glBindVertexArray(0); }

  void enable_attribute(GLuint index) const noexcept
  {
    bind();
    glEnableVertexAttribArray(index);
  }

  void disable_attribute(GLuint index) const noexcept
  {
    bind();
    glDisableVertexAttribArray(index);
  }

  template<numeric_type T>
  void set_attribute_pointer(GLuint index,
    GLint size,
    GLsizei stride,
    size_t offset_bytes,
    bool normalized = false) const noexcept
  {
    bind();
    glVertexAttribPointer(index,
      size,
      kGlTypeV<T>,
      normalized ? GL_TRUE : GL_FALSE,
      stride,
      // NOLINTNEXTLINE
      reinterpret_cast<void const *>(offset_bytes));
    glEnableVertexAttribArray(index);
  }

  template<numeric_type T>
  void set_attribute(GLuint index,
    GLint components,
    GLsizei stride,
    size_t offset_bytes,
    bool normalized = false) const noexcept
  {
    set_attribute_pointer<T>(
      index, components, stride, offset_bytes, normalized);
  }

  // Convenience methods for common types
  void set_float_attribute(GLuint index,
    GLint size,
    GLsizei stride,
    size_t offset_bytes) const noexcept
  {
    set_attribute<float>(index, size, stride, offset_bytes);
  }

  void set_int_attribute(GLuint index,
    GLint size,
    GLsizei stride,
    size_t offset_bytes) const noexcept
  {
    set_attribute<std::int32_t>(index, size, stride, offset_bytes);
  }

  void set_uint_attribute(GLuint index,
    GLint size,
    GLsizei stride,
    size_t offset_bytes) const noexcept
  {
    set_attribute<std::uint32_t>(index, size, stride, offset_bytes);
  }

  // Type-safe attribute setup for structs
  template<vertex_data VertexType, numeric_type MemberType>
  void set_member_attribute(GLuint index,
    GLint components,
    MemberType VertexType::*member,// NOLINT
    bool normalized = false) const noexcept
  {
    constexpr size_t kOffset = MemberOffset(member);
    set_attribute<MemberType>(
      index, components, sizeof(VertexType), kOffset, normalized);
  }

  void set_attribute_divisor(GLuint index, GLuint divisor) const noexcept
  {
    bind();
    glVertexAttribDivisor(index, divisor);
  }

  // Constexpr utilities for compile-time calculations
  template<vertex_data VertexType, numeric_type MemberType>
  static constexpr auto calculate_member_offset(
    MemberType VertexType::*member) noexcept -> std::size_t
  {
    return MemberOffset(member);
  }

  template<vertex_data VertexType>
  static constexpr auto calculate_stride() noexcept -> std::size_t
  {
    return sizeof(VertexType);
  }

  [[nodiscard]] constexpr auto get_id() const noexcept -> GLuint
  {
    return m_handle.get_id();
  }
  [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
  {
    return m_handle.is_valid();
  }

private:
  // NOLINTNEXTLINE
  constexpr explicit VertexArray(VertexArrayHandle &&handle) noexcept
    : m_handle{ std::move(handle) }
  {}
};

// RAII scoped binder
template<opengl_object T> class [[nodiscard]] ScopedBinder
{
private:
  T const &m_object;

public:
  constexpr explicit ScopedBinder(T const &obj) noexcept : m_object{ obj }
  {
    m_object.bind();
  }

  ~ScopedBinder() { m_object.unbind(); }

  ScopedBinder(ScopedBinder const &) = delete;
  auto operator=(ScopedBinder const &) -> ScopedBinder & = delete;
  ScopedBinder(ScopedBinder &&) = delete;
  auto operator=(ScopedBinder &&) -> ScopedBinder & = delete;
};

// Deduction guide
template<opengl_object T> ScopedBinder(T const &) -> ScopedBinder<T>;

// Factory function
template<opengl_object T>
[[nodiscard]] constexpr auto Bind(T const &object) noexcept
{
  return ScopedBinder{ object };
}

// Utility for creating vertex arrays with type-safe attribute binding
template<vertex_data VertexType> class VertexArrayBuilder
{
private:
  VertexArray m_vao;
  GLuint m_next_index{ 0 };

public:
  // NOLINTNEXTLINE
  constexpr explicit VertexArrayBuilder(VertexArray &&vao) noexcept
    : m_vao{ std::move(vao) }
  {}

  template<numeric_type MemberType>
  constexpr auto add_attribute(GLint components,
    MemberType VertexType::*member,
    bool normalized = false) noexcept -> VertexArrayBuilder &
  {
    m_vao.set_member_attribute<VertexType>(
      m_next_index, components, member, normalized);
    ++m_next_index;
    return *this;
  }

  constexpr auto set_divisor(GLuint divisor) noexcept -> VertexArrayBuilder &
  {
    if (m_next_index > 0) {
      m_vao.set_attribute_divisor(m_next_index - 1, divisor);
    }
    return *this;
  }

  [[nodiscard]] constexpr auto build() && noexcept -> VertexArray
  {
    return std::move(m_vao);
  }

  // Constexpr utilities for compile-time validation
  static consteval auto max_attributes() noexcept -> GLuint
  {
    // NOLINTNEXTLINE
    return 16;// Common OpenGL minimum
  }

  [[nodiscard]] constexpr auto current_attribute_count() const noexcept
    -> GLuint
  {
    return m_next_index;
  }

  [[nodiscard]] constexpr auto has_room_for_attributes(
    GLuint count) const noexcept -> bool
  {
    return (m_next_index + count) <= max_attributes();
  }
};

template<vertex_data VertexType>
[[nodiscard]] constexpr auto MakeVertexArrayBuilder()
  -> std::expected<VertexArrayBuilder<VertexType>, VertexArrayCreationError>
{
  auto Vao = VertexArray::create();
  if (!Vao) { return std::unexpected{ Vao.error() }; }
  return VertexArrayBuilder<VertexType>{ std::move(*Vao) };
}

// Constexpr attribute layout validation
template<vertex_data VertexType> struct AttributeLayout
{
  struct Attribute
  {
    std::size_t offset;
    std::size_t size;
    GLenum type;
    GLint components;
    bool normalized;


    // NOLINTNEXTLINE
    constexpr Attribute(std::size_t off,
      std::size_t sz,
      GLenum type,
      GLint comp,
      bool norm = false) noexcept
      : offset{ off }, size{ sz }, type{ type }, components{ comp },
        normalized{ norm }
    {}
  };

  static constexpr auto vertex_size() noexcept -> std::size_t
  {
    return sizeof(VertexType);
  }

  static constexpr auto vertex_alignment() noexcept -> std::size_t
  {
    return alignof(VertexType);
  }

  template<numeric_type MemberType>
  static constexpr auto make_attribute(std::size_t offset,
    GLint components,
    bool normalized = false) noexcept -> Attribute
  {
    return Attribute{
      offset, sizeof(MemberType), kGlTypeV<MemberType>, components, normalized
    };
  }

  template<numeric_type MemberType>
  static constexpr auto make_member_attribute(MemberType VertexType::*member,
    GLint components,
    bool normalized = false) noexcept -> Attribute
  {
    return make_attribute<MemberType>(
      MemberOffset(member), components, normalized);
  }
};

}// namespace gl
