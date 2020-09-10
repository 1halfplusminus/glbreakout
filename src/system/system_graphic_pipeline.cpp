#include "system_graphic_pipeline.hpp"
#include "component/component_render.hpp"
#include "component_graphic_pipeline.hpp"
#include "graphic/graphic_texture.hpp"
#include "helper_graphic_pipeline.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Graphic::init(entt::registry &registry)
{
  Graphic::update(registry, 0);
}

void Graphic::update(entt::registry &registry, float dt)
{
  entt::resource_cache<Graphic::ShaderProgam> program_cache;

  auto shaders = registry.view<ShaderProgam>();
  // update camera
  auto cameras = registry.view<ProjectionMatrix>();
  for (auto entity : cameras)
  {
    auto matrix = cameras.get<ProjectionMatrix>(entity);
    for (auto shader : shaders)
    {
      auto program = shaders.get<ShaderProgam>(shader);
      glUseProgram(program.id);
      glUniformMatrix4fv(glGetUniformLocation(program.id, "projection"), 1,
                         GL_FALSE, glm::value_ptr(matrix.value));
    }
  }
  // create buffer for sprite without buffer
  auto group =
      registry.group<Sprite>(entt::get<Position, Transform, Material>, entt::exclude<VertexBuffer>);
  for (auto entity : group)
  {
    auto [transform, renderable, position] =
        group.get<Transform, Sprite, Position>(
            entity);
    Graphic::VertexBuffer &bufferIndex =
        registry.emplace<Graphic::VertexBuffer>(entity);
    float offsetX = ((2 * (renderable.rect.x)) + 1) / (2 * renderable.texture.width);
    float offsetY = ((2 * (renderable.rect.y)) + 1) / (2 * renderable.texture.height);
    float ux = ((2 * (renderable.rect.z)) + 1) / (2 * renderable.texture.width);
    float uy = ((2 * (renderable.rect.w)) + 1) / (2 * renderable.texture.height);

    int uvSize = (renderable.vertices.size() / 4) * 2;
    std::vector<float> uvs(uvSize);
    for (int i = 0; i < renderable.vertices.size(); i += 4)
    {
      int y = (i / 4) * 2;
      uvs[y] = (renderable.vertices[i] * ux) + offsetX;
      uvs[y + 1] = (renderable.vertices[i + 1] * uy) + offsetY;
    }

    // configure VAO/VBO
    unsigned int VBO;
    // vertice
    glGenVertexArrays(1, &bufferIndex.id);
    glBindVertexArray(bufferIndex.id);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, renderable.vertices.size() * sizeof(float),
                 renderable.vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // uv
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float),
                 uvs.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // model
    glm::mat4 model = glm::mat4(1.0f);
    auto rotate = transform.rotate;
    auto size = transform.size;
    model = glm::translate(model, glm::vec3(position.value.x, position.value.y, 0.0f));
    model =
        glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model =
        glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model =
        glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));
    std::vector<glm::mat4> models = std::vector(renderable.vertices.size(), model);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, models.size() * sizeof(glm::mat4),
                 models.data(), GL_STREAM_DRAW);

    for (unsigned int i = 0; i < 4; i++)
    {
      glEnableVertexAttribArray(2 + i);
      glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                            (void *)(sizeof(glm::vec4) * i));
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
  }

  // renderable entity
  auto &renderable =
      registry.group<Sprite>(entt::get<Transform, Material>).proxy();
  int index = 0;
  for (auto &[entity, sprite, transform, mat] : renderable)
  {
    auto &buffer = registry.get<Graphic::VertexBuffer>(entity);
    if (index == 0)
    {
      glUseProgram(mat.shader);
    }
    glUniform3fv(glGetUniformLocation(mat.shader, "spriteColor"), 1,
                 glm::value_ptr(mat.color));
    glUniform1i(glGetUniformLocation(mat.shader, "image"), 0);
    glBindVertexArray(buffer.id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    index++;
  }
}