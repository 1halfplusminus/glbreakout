#include "system_graphic_pipeline.hpp"
#include "component/component_render.hpp"
#include "component_graphic_pipeline.hpp"
#include "graphic/graphic_texture.hpp"
#include "helper_graphic_pipeline.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Graphic::init(entt::registry &registry) {
  auto group = registry.group<Sprite>(entt::get<Position>);
  Graphic::update(registry, 0);
}

void Graphic::update(entt::registry &registry, float dt) {
  auto shaders = registry.view<ShaderProgam>();
  // update camera
  auto cameras = registry.view<ProjectionMatrix>();
  for (auto entity : cameras) {
    auto matrix = cameras.get<ProjectionMatrix>(entity);
    for (auto shader : shaders) {
      auto program = shaders.get<ShaderProgam>(shader);
      glUseProgram(program.id);
      glUniformMatrix4fv(glGetUniformLocation(program.id, "projection"), 1,
                         GL_FALSE, glm::value_ptr(matrix.value));
    }
  }
  // craete buffer for sprite without buffer
  auto group =
      registry.group<Sprite>(entt::get<Position>, entt::exclude<BufferIndex>);
  for (auto entity : group) {
    Graphic::Sprite renderable = group.get<Sprite>(entity);
    Graphic::BufferIndex &bufferIndex =
        registry.emplace<Graphic::BufferIndex>(entity);
    // configure VAO/VBO
    unsigned int VBO;

    glGenVertexArrays(1, &bufferIndex.id);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, renderable.vertices.size() * sizeof(float),
                 renderable.vertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(bufferIndex.id);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  // renderable entity
  auto renderable =
      registry.view<Position, BufferIndex, ShaderProgam, Texture, Transform>();
  for (auto entity : renderable) {
    auto [cPosition, transform, shader, texture, buffer] =
        renderable.get<Position, Transform, ShaderProgam, Texture, BufferIndex>(
            entity);
    glm::mat4 model = glm::mat4(1.0f);
    auto position = glm::vec2(cPosition.value.x, cPosition.value.y);
    auto rotate = transform.rotate;
    auto size = transform.size;
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model =
        glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model =
        glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model =
        glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    glUseProgram(shader.id);

    glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE,
                       glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shader.id, "spriteColor"), 1,
                 glm::value_ptr(transform.color));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glUniform1i(glGetUniformLocation(shader.id, "image"), 0);

    glBindVertexArray(buffer.id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
  }
}