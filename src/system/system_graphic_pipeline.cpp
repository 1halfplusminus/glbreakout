#include "system_graphic_pipeline.hpp"
#include "component/component_render.hpp"
#include "component_graphic_pipeline.hpp"
#include "graphic/graphic_texture.hpp"
#include "helper_graphic_pipeline.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

unsigned int vao;
unsigned int vbo[3];

namespace Graphic
{
  namespace
  {
    entt::registry graphicRegistry;
    void init_context(entt::registry &registry)
    {
      graphicRegistry.set<RenderContext>();
    }
    void create_vertex_array(entt::registry &registry, unsigned int &vao, unsigned int &vbo)
    {
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);
      glGenBuffers(1, &vbo);
      glBindVertexArray(vao);

      // vertex
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                            (void *)0);

      // uv
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                            (void *)offsetof(VertexData, uv));
      // color
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                            (void *)offsetof(VertexData, color));
      // model
      for (unsigned int i = 0; i < 4; i++)
      {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData),
                              (void *)(offsetof(VertexData, transform) + (sizeof(glm::vec4) * i)));
      }
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }
  } // namespace
} // namespace Graphic
entt::resource_handle<Graphic::Shader> Graphic::load_shader(const entt::hashed_string key, ShaderSource &source)
{
  if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
  {
    auto shader = ptr->shader_cache.load<Graphic::shader_loader>(
        key, source);
    return shader;
  }
  throw "render context not initialized !";
}
entt::resource_handle<Graphic::ShaderProgam> Graphic::load_shader_program(const entt::hashed_string key, std::vector<Graphic::Shader> &shaders)
{
  if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
  {
    auto shader = ptr->program_cache.load<Graphic::shader_program_loader>(
        key,
        shaders);
    // create a entity for the shader
    auto shader_entity = graphicRegistry.create();
    graphicRegistry.emplace<Graphic::ShaderProgam>(shader_entity, shader.get().id);
    return shader;
  }
  throw "render context not initialized !";
}
entt::resource_handle<Graphic::ShaderSource> Graphic::load_shader_source(
    const entt::hashed_string key,
    const std::string &path,
    const Graphic::ShaderType type)
{
  if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
  {
    auto source = ptr->shader_source_cache.load<Graphic::shader_source_loader>(
        key, path, type);
    return source;
  }
  throw "render context not initialized !";
}
entt::resource_handle<Graphic::Image> Graphic::load_image(const entt::hashed_string key, const std::string &path,
                                                          bool flipVertically)
{
  if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
  {
    auto image = ptr->image_cache.load<Graphic::image_loader>(key,
                                                              path, flipVertically);
    return image;
  }
  throw "render context not initialized !";
}
entt::resource_handle<Graphic::Texture> Graphic::load_texture(const entt::hashed_string key, Image &image)
{
  if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
  {
    auto &texture = ptr->texture_cache.load<Graphic::texture_image_loader>(
        "atlas"_hs, image);
    return texture;
  }
  throw "render context not initialized !";
}
void Graphic::init(entt::registry &registry)
{
  init_context(registry);
  create_vertex_array(registry, vao, *vbo);
  /*   Graphic::update(registry, 0); */
}

void Graphic::update(entt::registry &registry, float dt)
{
  entt::resource_cache<Graphic::ShaderProgam> program_cache;

  auto shaders = graphicRegistry.view<ShaderProgam>();

  auto group = registry.view<VertexData>();
  auto test = group.raw();
  for (auto &t : group)
  {
    /*   auto transform = group.get<Transform>(t);
    auto position = group.get<Position>(t); */
    /* registry.patch<Sprite>(t, [transform, position](auto &renderable) {
      float offsetX =
          ((2 * (renderable.rect.x)) + 1) / (2 * renderable.texture.width);
      float offsetY =
          ((2 * (renderable.rect.y)) + 1) / (2 * renderable.texture.height);
      float ux = ((2 * (renderable.rect.z)) + 1) / (2 * renderable.texture.width);
      float uy =
          ((2 * (renderable.rect.w)) + 1) / (2 * renderable.texture.height);
      int uvSize = (renderable.vertices.size() / 4) * 2;
      std::vector<float> uvs(uvSize);
      for (int i = 0; i < renderable.vertices.size(); i += 4)
      {
        int y = (i / 4) * 2;
        uvs[y] = (renderable.vertices[i] * ux) + offsetX;
        uvs[y + 1] = (renderable.vertices[i + 1] * uy) + offsetY;
      }
      renderable.uv = uvs;

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
      std::vector<glm::mat4> models =
          std::vector(renderable.vertices.size(), model);
      renderable.transforms = models; 
    }); */
  }
  if (group.size() > 0)
  {
    std::vector<VertexData> data;
    /*  for (int i = 0; i < group.size(); i++)
    {
      data.insert(
          data.end(),
          std::make_move_iterator(&(test[i].vertices[0])),
          std::make_move_iterator(&*(&test[i].vertices[0] + 1)));
    }
 */
    auto size = sizeof(test);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, group.size() * (sizeof(VertexData)),
                 test, GL_STATIC_DRAW);
    /* glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)offsetof(Sprite, vertices));
    glBindBuffer(GL_ARRAY_BUFFER, 0); */

    /*     glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, group.size() * sizeof(float), sprites, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, group.size() * sizeof(float), sprites, GL_STATIC_DRAW); */
  }

  // create buffer for sprite without buffer
  /*   auto group = registry.group<Sprite>(entt::get<Position, Transform, Material>,
                                      entt::exclude<VertexBuffer>);
  for (auto entity : group)
  {
    auto [transform, renderable, position] =
        group.get<Transform, Sprite, Position>(entity);
    Graphic::VertexBuffer &bufferIndex =
        registry.emplace<Graphic::VertexBuffer>(entity);
    float offsetX =
        ((2 * (renderable.rect.x)) + 1) / (2 * renderable.texture.width);
    float offsetY =
        ((2 * (renderable.rect.y)) + 1) / (2 * renderable.texture.height);
    float ux = ((2 * (renderable.rect.z)) + 1) / (2 * renderable.texture.width);
    float uy =
        ((2 * (renderable.rect.w)) + 1) / (2 * renderable.texture.height);

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
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(),
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          (void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

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
    std::vector<glm::mat4> models =
        std::vector(renderable.vertices.size(), model);
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
  } */

  // renderable entity
  /*  auto &renderable =
      registry.group<Sprite>(entt::get<Transform, Material>).proxy();
  int index = 0;
  int spriteColorLocation;
  int imageLocation; */
  /* for (auto &[entity, sprite, transform, mat] : renderable)
  {

    if (index == 0)
    {
      glUseProgram(mat.shader);
      spriteColorLocation = glGetUniformLocation(mat.shader, "spriteColor");
      imageLocation = glGetUniformLocation(sprite.texture.id, "image");
    }

    glUniform3fv(spriteColorLocation, 1, glm::value_ptr(mat.color));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite.texture.id);
    glUniform1i(imageLocation, 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    index++;
  } */
  // update camera
  auto cameras = registry.view<ProjectionMatrix>();
  auto textures = registry.view<Sprite>();
  for (auto entity : cameras)
  {
    auto matrix = cameras.get<ProjectionMatrix>(entity);
    for (auto e : shaders)
    {

      auto shader = shaders.get<ShaderProgam>(e);
      auto t = registry.get<Sprite>(textures[0]);
      glUseProgram(shader.id);
      glUniformMatrix4fv(glGetUniformLocation(shader.id, "projection"), 1,
                         GL_FALSE, glm::value_ptr(matrix.value));
      auto imageLocation = glGetUniformLocation(shader.id, "image");
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, t.texture.id);
      glUniform1i(imageLocation, 0);
      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, group.size());
      glBindVertexArray(0);
    }
  }
}