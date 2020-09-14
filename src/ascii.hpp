

#pragma once

#include <glm/glm.hpp>
#include <map>
#include <string>

// text renderer
namespace ascii {

/**
 * @brief vertext shader source GLSL code
 */
static const char *vert_shader =
    "#version 330 core"
    "in vec2 TexCoords;"
    "out vec4 color;"
    "uniform sampler2D text;"
    "uniform vec3 textColor;"
    "void main() {"
    "  vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);"
    "  color = vec4(textColor, 1.0) * sampled;"
    "}";

/**
 * @brief fragment shader source GLSL code
 */
static const char *frag_shader =
    "#version 330 core"
    "layout(location = 0) in vec4 vertex;"
    "out vec2 TexCoords;"
    "uniform mat4 projection;"
    "void main() {"
    "  gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);"
    "  TexCoords = vertex.zw;"
    "}";

/**
 * @brief Abastract a character object
 */
struct character {
  /**
   * @brief ID handle of the glyph texture
   */
  GLuint texture;

  /**
   * @brief size of glyph
   */
  glm::ivec2 size;

  /**
   * @brief Offset from baseline to left/top of glyph
   */
  glm::ivec2 bearing;
  /**
   * @brief Horizontal offset to advance to next glyph
   */
  unsigned int advance;
};

// texture init
auto init_characters(char const *font) -> std::map<GLchar, character>;

class renderer {
 public:
  renderer(GLuint const &program, std::map<GLchar, character> const &characters,
	   float height, float width);

  /**
   * @brief Render a text to a framebuffer
   *
   * @param text which going to be rendered
   * @param x Position to render on the screen
   * @param y Position to render on the screen
   * @param scale A factor to scale a text
   * @param color for a text.
   *
   * @return None
   */
  auto render(std::string const &text, float x, float y, float scale,
	      glm::vec3 color) -> void;

  /**
   * @brief Update the screen size
   *
   * @param width screen width
   * @param height screen height
   *
   * @return None
   */
  auto update_size(float width, float height) -> void;

 private:
  std::map<GLchar, character> characters_;
  GLuint vao_, vbo_;
  GLuint program_;

  auto init_shader(float width, float height) -> void;
  auto init_vertices() -> void;
};

auto init_renderer(GLuint shader, float width, float height) -> renderer;
}
