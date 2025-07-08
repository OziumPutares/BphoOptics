#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

// Mock OpenGL functions for testing
#include <GL/gl.h>
#include <algorithm>
#include <unordered_set>
#include <vector>

// Mock OpenGL state
namespace mock_gl {
static std::unordered_set<GLuint> allocated_buffers;
static std::unordered_set<GLuint> allocated_vertex_arrays;
static std::unordered_set<GLuint> bound_buffers;
static std::unordered_set<GLuint> bound_vertex_arrays;
static GLuint next_id = 1;
static bool should_fail_allocation = false;

void reset_state()
{
  allocated_buffers.clear();
  allocated_vertex_arrays.clear();
  bound_buffers.clear();
  bound_vertex_arrays.clear();
  next_id = 1;
  should_fail_allocation = false;
}

void set_allocation_failure(bool should_fail)
{
  should_fail_allocation = should_fail;
}
}// namespace mock_gl

// Mock OpenGL functions
extern "C" {
void glGenBuffers(GLsizei n, GLuint *buffers)
{
  if (mock_gl::should_fail_allocation) {
    for (GLsizei i = 0; i < n; ++i) { buffers[i] = 0; }
    return;
  }

  for (GLsizei i = 0; i < n; ++i) {
    buffers[i] = mock_gl::next_id++;
    mock_gl::allocated_buffers.insert(buffers[i]);
  }
}

void glDeleteBuffers(GLsizei n, GLuint const *buffers)
{
  for (GLsizei i = 0; i < n; ++i) {
    mock_gl::allocated_buffers.erase(buffers[i]);
    mock_gl::bound_buffers.erase(buffers[i]);
  }
}

void glBindBuffer(GLenum target, GLuint buffer)
{
  if (buffer == 0) {
    // Unbind
    mock_gl::bound_buffers.clear();
  } else {
    mock_gl::bound_buffers.insert(buffer);
  }
}

void glGenVertexArrays(GLsizei n, GLuint *arrays)
{
  if (mock_gl::should_fail_allocation) {
    for (GLsizei i = 0; i < n; ++i) { arrays[i] = 0; }
    return;
  }

  for (GLsizei i = 0; i < n; ++i) {
    arrays[i] = mock_gl::next_id++;
    mock_gl::allocated_vertex_arrays.insert(arrays[i]);
  }
}

void glDeleteVertexArrays(GLsizei n, GLuint const *arrays)
{
  for (GLsizei i = 0; i < n; ++i) {
    mock_gl::allocated_vertex_arrays.erase(arrays[i]);
    mock_gl::bound_vertex_arrays.erase(arrays[i]);
  }
}

void glBindVertexArray(GLuint array)
{
  if (array == 0) {
    mock_gl::bound_vertex_arrays.clear();
  } else {
    mock_gl::bound_vertex_arrays.insert(array);
  }
}

void glBufferData(GLenum target,
  GLsizeiptr size,
  void const *data,
  GLenum usage)
{
  // Mock implementation - just validate we have a bound buffer
  REQUIRE(!mock_gl::bound_buffers.empty());
}

void glBufferSubData(GLenum target,
  GLintptr offset,
  GLsizeiptr size,
  void const *data)
{
  // Mock implementation - just validate we have a bound buffer
  REQUIRE(!mock_gl::bound_buffers.empty());
}

void glVertexAttribPointer(GLuint index,
  GLint size,
  GLenum type,
  GLboolean normalized,
  GLsizei stride,
  void const *pointer)
{
  // Mock implementation
}

void glEnableVertexAttribArray(GLuint index)
{
  // Mock implementation
}

void glDisableVertexAttribArray(GLuint index)
{
  // Mock implementation
}

void glVertexAttribDivisor(GLuint index, GLuint divisor)
{
  // Mock implementation
}
}

// Test vertex data structure
struct TestVertex
{
  float position[3];
  float color[4];
  float texCoords[2];
  int id;
};

// Test fixture
class OpenGLWrapperTest
{
public:
  OpenGLWrapperTest() { mock_gl::reset_state(); }

  ~OpenGLWrapperTest() { mock_gl::reset_state(); }
};

// Compile-time tests
TEST_CASE("Constexpr Type Mapping", "[gl][constexpr]")
{
  SECTION("GL type constants are correct")
  {
    STATIC_REQUIRE(gl::kGlTypeV<float> == GL_FLOAT);
    STATIC_REQUIRE(gl::kGlTypeV<double> == GL_DOUBLE);
    STATIC_REQUIRE(gl::kGlTypeV<std::int8_t> == GL_BYTE);
    STATIC_REQUIRE(gl::kGlTypeV<std::uint8_t> == GL_UNSIGNED_BYTE);
    STATIC_REQUIRE(gl::kGlTypeV<std::int16_t> == GL_SHORT);
    STATIC_REQUIRE(gl::kGlTypeV<std::uint16_t> == GL_UNSIGNED_SHORT);
    STATIC_REQUIRE(gl::kGlTypeV<std::int32_t> == GL_INT);
    STATIC_REQUIRE(gl::kGlTypeV<std::uint32_t> == GL_UNSIGNED_INT);
  }

  SECTION("Utility functions are constexpr")
  {
    STATIC_REQUIRE(gl::gl_type_size<float>() == sizeof(float));
    STATIC_REQUIRE(gl::gl_type_alignment<float>() == alignof(float));
    STATIC_REQUIRE(gl::vertex_size<TestVertex>() == sizeof(TestVertex));
    STATIC_REQUIRE(gl::vertex_alignment<TestVertex>() == alignof(TestVertex));
  }

  SECTION("Member offset calculation")
  {
    STATIC_REQUIRE(gl::member_offset(&TestVertex::position)
                   == offsetof(TestVertex, position));
    STATIC_REQUIRE(
      gl::member_offset(&TestVertex::color) == offsetof(TestVertex, color));
    STATIC_REQUIRE(gl::member_offset(&TestVertex::texCoords)
                   == offsetof(TestVertex, texCoords));
    STATIC_REQUIRE(
      gl::member_offset(&TestVertex::id) == offsetof(TestVertex, id));
  }
}

TEST_CASE("Error Types", "[gl][error]")
{
  SECTION("Error messages are constexpr")
  {
    STATIC_REQUIRE(
      gl::BufferCreationError::message() == "Failed to create OpenGL buffer");
    STATIC_REQUIRE(gl::VertexArrayCreationError::message()
                   == "Failed to create OpenGL vertex array");
    STATIC_REQUIRE(
      gl::InvalidOperationError::message() == "Invalid OpenGL operation");
  }
}

TEST_CASE("Handle Classes", "[gl][handle]")
{
  OpenGLWrapperTest fixture;

  SECTION("BufferHandle basic operations")
  {
    auto handle = gl::BufferHandle{ 42 };

    REQUIRE(handle.get_id() == 42);
    REQUIRE(handle.is_valid());
    REQUIRE(static_cast<bool>(handle));

    // Test invalid handle
    auto invalid_handle = gl::BufferHandle{ 0 };
    REQUIRE(!invalid_handle.is_valid());
    REQUIRE(!static_cast<bool>(invalid_handle));
  }

  SECTION("BufferHandle move semantics")
  {
    auto handle1 = gl::BufferHandle{ 42 };
    auto handle2 = std::move(handle1);

    REQUIRE(handle2.get_id() == 42);
    REQUIRE(handle2.is_valid());
    REQUIRE(handle1.get_id() == 0);// Moved from
    REQUIRE(!handle1.is_valid());

    // Test move assignment
    auto handle3 = gl::BufferHandle{ 100 };
    handle3 = std::move(handle2);

    REQUIRE(handle3.get_id() == 42);
    REQUIRE(handle2.get_id() == 0);// Moved from
  }

  SECTION("VertexArrayHandle basic operations")
  {
    auto handle = gl::VertexArrayHandle{ 42 };

    REQUIRE(handle.get_id() == 42);
    REQUIRE(handle.is_valid());
    REQUIRE(static_cast<bool>(handle));

    // Test default construction
    auto default_handle = gl::VertexArrayHandle{};
    REQUIRE(default_handle.get_id() == 0);
    REQUIRE(!default_handle.is_valid());
  }

  SECTION("VertexArrayHandle move semantics")
  {
    auto handle1 = gl::VertexArrayHandle{ 42 };
    auto handle2 = std::move(handle1);

    REQUIRE(handle2.get_id() == 42);
    REQUIRE(handle2.is_valid());
    REQUIRE(handle1.get_id() == 0);
    REQUIRE(!handle1.is_valid());
  }
}

TEST_CASE("VertexBuffer", "[gl][buffer]")
{
  OpenGLWrapperTest fixture;

  SECTION("Successful creation")
  {
    auto buffer_result = gl::VertexBuffer::create();
    REQUIRE(buffer_result.has_value());

    auto &buffer = buffer_result.value();
    REQUIRE(buffer.is_valid());
    REQUIRE(buffer.get_id() != 0);
    REQUIRE(buffer.get_target() == GL_ARRAY_BUFFER);
    REQUIRE(mock_gl::allocated_buffers.contains(buffer.get_id()));
  }

  SECTION("Creation with custom target")
  {
    auto buffer_result = gl::VertexBuffer::create(GL_ELEMENT_ARRAY_BUFFER);
    REQUIRE(buffer_result.has_value());

    auto &buffer = buffer_result.value();
    REQUIRE(buffer.get_target() == GL_ELEMENT_ARRAY_BUFFER);
  }

  SECTION("Failed creation")
  {
    mock_gl::set_allocation_failure(true);
    auto buffer_result = gl::VertexBuffer::create();
    REQUIRE(!buffer_result.has_value());
    REQUIRE(
      buffer_result.error().message() == "Failed to create OpenGL buffer");
  }

  SECTION("Buffer operations")
  {
    auto buffer = gl::VertexBuffer::create().value();
    GLuint buffer_id = buffer.get_id();

    // Test binding
    buffer.bind();
    REQUIRE(mock_gl::bound_buffers.contains(buffer_id));

    // Test unbinding
    buffer.unbind();
    REQUIRE(mock_gl::bound_buffers.empty());

    // Test data setting
    std::vector<TestVertex> vertices = {
      { { 1.0f, 2.0f, 3.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, 1 },
      { { 4.0f, 5.0f, 6.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, 2 }
    };

    REQUIRE_NOTHROW(buffer.set_data(vertices));
    REQUIRE_NOTHROW(buffer.set_data(std::span{ vertices }));

    // Test sub data
    REQUIRE_NOTHROW(buffer.set_sub_data(std::span{ vertices }, 0));
    REQUIRE_NOTHROW(
      buffer.set_sub_data(std::span{ vertices }, 1));// offset in elements
  }

  SECTION("Constexpr buffer calculations")
  {
    STATIC_REQUIRE(gl::VertexBuffer::calculate_buffer_size<TestVertex>(10)
                   == 10 * sizeof(TestVertex));
    STATIC_REQUIRE(gl::VertexBuffer::calculate_element_offset<TestVertex>(5)
                   == 5 * sizeof(TestVertex));
  }

  SECTION("RAII cleanup")
  {
    GLuint buffer_id;
    {
      auto buffer = gl::VertexBuffer::create().value();
      buffer_id = buffer.get_id();
      REQUIRE(mock_gl::allocated_buffers.contains(buffer_id));
    }
    // Buffer should be cleaned up automatically
    REQUIRE(!mock_gl::allocated_buffers.contains(buffer_id));
  }
}

TEST_CASE("VertexArray", "[gl][vertex_array]")
{
  OpenGLWrapperTest fixture;

  SECTION("Successful creation")
  {
    auto vao_result = gl::VertexArray::create();
    REQUIRE(vao_result.has_value());

    auto &vao = vao_result.value();
    REQUIRE(vao.is_valid());
    REQUIRE(vao.get_id() != 0);
    REQUIRE(mock_gl::allocated_vertex_arrays.contains(vao.get_id()));
  }

  SECTION("Failed creation")
  {
    mock_gl::set_allocation_failure(true);
    auto vao_result = gl::VertexArray::create();
    REQUIRE(!vao_result.has_value());
    REQUIRE(
      vao_result.error().message() == "Failed to create OpenGL vertex array");
  }

  SECTION("Binding operations")
  {
    auto vao = gl::VertexArray::create().value();
    GLuint vao_id = vao.get_id();

    // Test binding
    vao.bind();
    REQUIRE(mock_gl::bound_vertex_arrays.contains(vao_id));

    // Test static unbinding
    gl::VertexArray::unbind();
    REQUIRE(mock_gl::bound_vertex_arrays.empty());
  }

  SECTION("Attribute operations")
  {
    auto vao = gl::VertexArray::create().value();

    // These should not throw (they call mock OpenGL functions)
    REQUIRE_NOTHROW(vao.enable_attribute(0));
    REQUIRE_NOTHROW(vao.disable_attribute(0));
    REQUIRE_NOTHROW(vao.set_attribute_divisor(0, 1));

    // Test attribute pointer setup
    REQUIRE_NOTHROW(
      vao.set_attribute_pointer<float>(0, 3, sizeof(TestVertex), 0));
    REQUIRE_NOTHROW(vao.set_attribute<float>(
      1, 4, sizeof(TestVertex), offsetof(TestVertex, color)));

    // Test convenience methods
    REQUIRE_NOTHROW(vao.set_float_attribute(0, 3, sizeof(TestVertex), 0));
    REQUIRE_NOTHROW(vao.set_int_attribute(
      1, 1, sizeof(TestVertex), offsetof(TestVertex, id)));
    REQUIRE_NOTHROW(vao.set_uint_attribute(
      2, 1, sizeof(TestVertex), offsetof(TestVertex, id)));

    // Test member attribute setup
    REQUIRE_NOTHROW(vao.set_member_attribute(0, 3, &TestVertex::position));
    REQUIRE_NOTHROW(vao.set_member_attribute(1, 4, &TestVertex::color));
    REQUIRE_NOTHROW(vao.set_member_attribute(2, 2, &TestVertex::texCoords));
    REQUIRE_NOTHROW(vao.set_member_attribute(3, 1, &TestVertex::id));
  }

  SECTION("Constexpr calculations")
  {
    STATIC_REQUIRE(
      gl::VertexArray::calculate_member_offset(&TestVertex::position)
      == offsetof(TestVertex, position));
    STATIC_REQUIRE(gl::VertexArray::calculate_member_offset(&TestVertex::color)
                   == offsetof(TestVertex, color));
    STATIC_REQUIRE(
      gl::VertexArray::calculate_stride<TestVertex>() == sizeof(TestVertex));
  }

  SECTION("RAII cleanup")
  {
    GLuint vao_id;
    {
      auto vao = gl::VertexArray::create().value();
      vao_id = vao.get_id();
      REQUIRE(mock_gl::allocated_vertex_arrays.contains(vao_id));
    }
    // VAO should be cleaned up automatically
    REQUIRE(!mock_gl::allocated_vertex_arrays.contains(vao_id));
  }
}

TEST_CASE("ScopedBinder", "[gl][scoped_binder]")
{
  OpenGLWrapperTest fixture;

  SECTION("RAII binding with VertexBuffer")
  {
    auto buffer = gl::VertexBuffer::create().value();
    GLuint buffer_id = buffer.get_id();

    {
      auto binder = gl::ScopedBinder{ buffer };
      REQUIRE(mock_gl::bound_buffers.contains(buffer_id));
    }
    // Should be unbound after scope
    REQUIRE(mock_gl::bound_buffers.empty());
  }

  SECTION("RAII binding with VertexArray")
  {
    auto vao = gl::VertexArray::create().value();
    GLuint vao_id = vao.get_id();

    {
      auto binder = gl::ScopedBinder{ vao };
      REQUIRE(mock_gl::bound_vertex_arrays.contains(vao_id));
    }
    // Should be unbound after scope
    REQUIRE(mock_gl::bound_vertex_arrays.empty());
  }

  SECTION("Factory function")
  {
    auto buffer = gl::VertexBuffer::create().value();
    GLuint buffer_id = buffer.get_id();

    {
      auto binder = gl::Bind(buffer);
      REQUIRE(mock_gl::bound_buffers.contains(buffer_id));
    }
    REQUIRE(mock_gl::bound_buffers.empty());
  }
}

TEST_CASE("VertexArrayBuilder", "[gl][builder]")
{
  OpenGLWrapperTest fixture;

  SECTION("Basic building")
  {
    auto builder_result = gl::MakeVertexArrayBuilder<TestVertex>();
    REQUIRE(builder_result.has_value());

    auto vao = std::move(builder_result.value())
                 .add_attribute(3, &TestVertex::position)
                 .add_attribute(4, &TestVertex::color)
                 .add_attribute(2, &TestVertex::texCoords)
                 .add_attribute(1, &TestVertex::id)
                 .build();

    REQUIRE(vao.is_valid());
  }

  SECTION("Builder with divisor")
  {
    auto builder_result = gl::MakeVertexArrayBuilder<TestVertex>();
    REQUIRE(builder_result.has_value());

    auto vao = std::move(builder_result.value())
                 .add_attribute(3, &TestVertex::position)
                 .set_divisor(1)
                 .add_attribute(4, &TestVertex::color)
                 .build();

    REQUIRE(vao.is_valid());
  }

  SECTION("Builder state tracking")
  {
    auto builder_result = gl::MakeVertexArrayBuilder<TestVertex>();
    REQUIRE(builder_result.has_value());

    auto &builder = builder_result.value();
    REQUIRE(builder.current_attribute_count() == 0);
    REQUIRE(builder.has_room_for_attributes(16));

    builder.add_attribute(3, &TestVertex::position);
    REQUIRE(builder.current_attribute_count() == 1);
    REQUIRE(builder.has_room_for_attributes(15));
  }

  SECTION("Constexpr builder utilities")
  {
    using Builder = gl::VertexArrayBuilder<TestVertex>;
    STATIC_REQUIRE(Builder::max_attributes() == 16);
  }

  SECTION("Failed builder creation")
  {
    mock_gl::set_allocation_failure(true);
    auto builder_result = gl::MakeVertexArrayBuilder<TestVertex>();
    REQUIRE(!builder_result.has_value());
  }
}

TEST_CASE("AttributeLayout", "[gl][attribute_layout]")
{
  SECTION("Basic layout information")
  {
    using Layout = gl::AttributeLayout<TestVertex>;

    STATIC_REQUIRE(Layout::vertex_size() == sizeof(TestVertex));
    STATIC_REQUIRE(Layout::vertex_alignment() == alignof(TestVertex));
  }

  SECTION("Attribute creation")
  {
    using Layout = gl::AttributeLayout<TestVertex>;

    constexpr auto pos_attr =
      Layout::make_member_attribute(&TestVertex::position, 3);
    STATIC_REQUIRE(pos_attr.offset == offsetof(TestVertex, position));
    STATIC_REQUIRE(pos_attr.size == sizeof(float));
    STATIC_REQUIRE(pos_attr.type == GL_FLOAT);
    STATIC_REQUIRE(pos_attr.components == 3);
    STATIC_REQUIRE(pos_attr.normalized == false);

    constexpr auto color_attr =
      Layout::make_member_attribute(&TestVertex::color, 4, true);
    STATIC_REQUIRE(color_attr.offset == offsetof(TestVertex, color));
    STATIC_REQUIRE(color_attr.normalized == true);
  }

  SECTION("Manual attribute creation")
  {
    using Layout = gl::AttributeLayout<TestVertex>;

    constexpr auto manual_attr = Layout::make_attribute<float>(16, 2);
    STATIC_REQUIRE(manual_attr.offset == 16);
    STATIC_REQUIRE(manual_attr.size == sizeof(float));
    STATIC_REQUIRE(manual_attr.type == GL_FLOAT);
    STATIC_REQUIRE(manual_attr.components == 2);
  }
}

TEST_CASE("Concepts", "[gl][concepts]")
{
  SECTION("vertex_data concept")
  {
    STATIC_REQUIRE(gl::vertex_data<TestVertex>);
    STATIC_REQUIRE(gl::vertex_data<float>);
    STATIC_REQUIRE(gl::vertex_data<int>);
    STATIC_REQUIRE(!gl::vertex_data<std::byte>);
  }

  SECTION("numeric_type concept")
  {
    STATIC_REQUIRE(gl::numeric_type<float>);
    STATIC_REQUIRE(gl::numeric_type<int>);
    STATIC_REQUIRE(gl::numeric_type<std::uint32_t>);
    STATIC_REQUIRE(!gl::numeric_type<std::string>);
  }

  SECTION("opengl_object concept")
  {
    STATIC_REQUIRE(gl::opengl_object<gl::VertexBuffer>);
    STATIC_REQUIRE(gl::opengl_object<gl::VertexArray>);
  }
}

TEST_CASE("Integration Test", "[gl][integration]")
{
  OpenGLWrapperTest fixture;

  SECTION("Complete vertex setup workflow")
  {
    // Create buffer and VAO
    auto buffer = gl::VertexBuffer::create().value();
    auto vao = gl::VertexArray::create().value();

    // Test data
    std::vector<TestVertex> vertices = {
      { { 0.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f }, 1 },
      { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, 2 },
      { { 0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f }, 3 }
    };

    // Setup buffer data
    buffer.set_data(vertices);

    // Setup vertex attributes using scoped binding
    {
      auto buffer_bind = gl::Bind(buffer);
      auto vao_bind = gl::Bind(vao);

      vao.set_member_attribute(0, 3, &TestVertex::position);
      vao.set_member_attribute(1, 4, &TestVertex::color);
      vao.set_member_attribute(2, 2, &TestVertex::texCoords);
      vao.set_member_attribute(3, 1, &TestVertex::id);
    }

    // Verify objects are still valid
    REQUIRE(buffer.is_valid());
    REQUIRE(vao.is_valid());

    // Verify cleanup happens automatically when objects go out of scope
    GLuint buffer_id = buffer.get_id();
    GLuint vao_id = vao.get_id();

    REQUIRE(mock_gl::allocated_buffers.contains(buffer_id));
    REQUIRE(mock_gl::allocated_vertex_arrays.contains(vao_id));
  }

  SECTION("Builder pattern workflow")
  {
    auto buffer = gl::VertexBuffer::create().value();

    std::vector<TestVertex> vertices = {
      { { 0.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f }, 1 }
    };

    buffer.set_data(vertices);

    auto vao = gl::MakeVertexArrayBuilder<TestVertex>()
                 .value()
                 .add_attribute(3, &TestVertex::position)
                 .add_attribute(4, &TestVertex::color)
                 .add_attribute(2, &TestVertex::texCoords)
                 .add_attribute(1, &TestVertex::id)
                 .build();

    REQUIRE(vao.is_valid());
  }
}
