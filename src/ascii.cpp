
#include "ascii.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ascii {

auto init_characters(char const *font) -> std::map<GLchar, character> {
  std::map<GLchar, character> characters;

  FT_Library ft;
  // All functions return a value different than 0 whenever an error occurred
  if (FT_Init_FreeType(&ft))
    throw std::runtime_error("could not init freetype lib");

  // load font as face
  FT_Face face;
  if (FT_New_Face(ft, font, 0, &face))
    throw std::runtime_error("failed to load font");

  // set size to load glyphs as
  FT_Set_Pixel_Sizes(face, 0, 48);

  // disable byte-alignment restriction
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // load first 128 characters of ASCII set
  for (unsigned char c = 0; c < 128; c++) {
    // Load character glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      throw std::runtime_error("failed to load glyph");
    }
    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
		 face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
		 face->glyph->bitmap.buffer);
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // now store character for later use
    character ch = {
	texture,
	glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
	glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
	face->glyph->advance.x};
    characters.insert(std::pair<char, character>(c, ch));
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  // destroy FreeType once we're finished
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  return characters;
}

renderer::renderer(GLuint const &program,
		   std::map<GLchar, character> const &characters, float width,
		   float height)
    : program_(program), characters_(characters) {
  init_shader(width, height);
  init_vertices();
}

auto renderer::init_shader(float width, float height) -> void {
  glUseProgram(program_);

  glm::mat4 projection = glm::ortho(0.0f, width, 0.0f, height);

  glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1, GL_FALSE,
		     glm::value_ptr(projection));
}

auto renderer::init_vertices() -> void {
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

auto renderer::render(std::string const &text, float x, float y, float scale,
		      glm::vec3 color) -> void {
  // shader.use();
  glUseProgram(program_);

  glUniform3f(glGetUniformLocation(program_, "textColor"), color.x, color.y,
	      color.z);
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(vao_);

  // iterate through all characters
  std::string::const_iterator c;
  for (c = text.begin(); c != text.end(); c++) {
    auto ch = characters_[*c];

    float xpos = x + ch.bearing.x * scale;
    float ypos = y - (ch.size.y - ch.bearing.y) * scale;

    float w = ch.size.x * scale;
    float h = ch.size.y * scale;
    // update VBO for each character
    float vertices[6][4] = {
	{xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
	{xpos + w, ypos, 1.0f, 1.0f},

	{xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
	{xpos + w, ypos + h, 1.0f, 0.0f}};
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.texture);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(
	GL_ARRAY_BUFFER, 0, sizeof(vertices),
	vertices);  // be sure to use glBufferSubData and not glBufferData

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph (note that advance is number of 1/64
    // pixels)
    x += (ch.advance >> 6) * scale;  // bitshift by 6 to get value in pixels
				     // (2^6 = 64 (divide amount of 1/64th
				     // pixels by 64 to get amount of pixels))
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

auto init_renderer(GLuint shader, float width, float height) -> renderer {
  // default values
  return renderer(shader, init_characters("fonts/arial.ttf"), width, height);
}
}

