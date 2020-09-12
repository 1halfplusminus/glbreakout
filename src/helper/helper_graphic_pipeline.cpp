#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "helper_graphic_pipeline.hpp"
#include "ecs.h"
#include "component/component_render.hpp"
#include <glm/gtc/matrix_transform.hpp>

std::shared_ptr<Graphic::ShaderSource>
Graphic::shader_source_loader::load(std::string path,
                                    Graphic::ShaderType type) const
{
  std::ifstream ifs(path.c_str(),
                    std::ios::in | std::ios::binary | std::ios::ate);
  auto source =
      std::shared_ptr<Graphic::ShaderSource>(new Graphic::ShaderSource{});
  source->type = type;
  if (!ifs || ifs.bad())
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << path
              << std::endl;
  }
  else
  {
    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    source->source.reserve(ifs.tellg());
    ifs.seekg(0, std::ios::beg);

    source->source.assign((std::istreambuf_iterator<char>(ifs)),
                          std::istreambuf_iterator<char>());
  }

  return source;
}

std::shared_ptr<Graphic::Shader>
Graphic::shader_loader::load(ShaderSource &shaderSource) const
{

  // build and compile our shader program
  // ------------------------------------
  // check for shader compile errors

  unsigned int shader;
  const GLint size = (int)shaderSource.source.size();
  const GLchar *vertexShaderSourceChar = shaderSource.source.c_str();
  shader =
      glCreateShader(shaderSource.type == VertexShader ? GL_VERTEX_SHADER
                                                       : GL_FRAGMENT_SHADER);
  glShaderSource(shader, 1, &vertexShaderSourceChar, &size);
  glCompileShader(shader);
  int success;
  char infoLog[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
              << (shaderSource.type == VertexShader ? "VERTEX" : "FRAGMENT")
              << "\n"
              << infoLog
              << "\n -- --------------------------------------------------- -- "
              << std::endl;
  }
  return std::shared_ptr<Graphic::Shader>(new Graphic::Shader{shader});
}

std::shared_ptr<Graphic::ShaderProgam>
Graphic::shader_program_loader::load(std::vector<Shader> &shaders) const
{
  // link shaders
  unsigned int ID = glCreateProgram();
  for (auto &shader : shaders)
  {
    glAttachShader(ID, shader.id);
  }
  glLinkProgram(ID);
  int success;
  char infoLog[1024];
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(ID, 1024, NULL, infoLog);
    std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
              << "PROGRAM"
              << "\n"
              << infoLog
              << "\n -- --------------------------------------------------- -- "
              << std::endl;
  }
  return std::shared_ptr<Graphic::ShaderProgam>(new Graphic::ShaderProgam{ID});
}

glm::mat4 Graphic::create_model_matrix(const Position &position, const Transform &transform)
{
  // model
  glm::mat4 model = glm::mat4(1.0f);
  auto rotate = transform.rotate;
  auto size = transform.size;
  model = glm::translate(model,
                         glm::vec3(position.value.x, position.value.y, 0.0f));
  model =
      glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
  model =
      glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
  model =
      glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

  model = glm::scale(model, glm::vec3(size, 1.0f));
  return model;
}

glm::vec2 Graphic::calculate_uv(const Sprite &sprite, const VertexData &vertex)
{
  float offsetX =
      ((2 * (sprite.rect.x)) + 1) / (2 * sprite.texture.width);
  float offsetY =
      ((2 * (sprite.rect.y)) + 1) / (2 * sprite.texture.height);
  float ux = ((2 * (sprite.rect.z)) + 1) / (2 * sprite.texture.width);
  float uy =
      ((2 * (sprite.rect.w)) + 1) / (2 * sprite.texture.height);
  glm::vec2 uv;
  uv.x = (vertex.vertice.x * ux) + offsetX;
  uv.y = (vertex.vertice.y * uy) + offsetY;
  return uv;
}