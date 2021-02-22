#include "graphic/system_graphic_pipeline.hpp"
#include "graphic/component_render.hpp"
#include "graphic/component_graphic_pipeline.hpp"
#include "graphic/graphic_texture.hpp"
#include "graphic/helper_graphic_pipeline.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Graphic
{
  namespace
  {
    constexpr entt::hashed_string vbo_tag = entt::hashed_string("vbo");
    constexpr entt::hashed_string vao_tag = entt::hashed_string("vao");
    entt::registry graphicRegistry;

    void handle_error_context()
    {
      std::cout << "render context not initialized !" << std::endl;
      throw std::invalid_argument::invalid_argument(
          "render context not initialized !");
    }
    void init_context(entt::registry &registry)
    {
      auto &context = graphicRegistry.set<RenderContext>();
      context.position_observer = std::unique_ptr<entt::observer>(
          new entt::observer(registry, entt::collector.update<Position>().update<Transform>().where<RenderGroupHandle, Transform>()));
    }

    void create_vertex_array(entt::registry &registry, unsigned int &vao,
                             unsigned int &vbo)
    {
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);
      glGenBuffers(1, &vbo);

      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      // vertex
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
        glVertexAttribPointer(
            3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData),
            (void *)(offsetof(VertexData, transform) + (sizeof(glm::vec4) * i)));
      }

      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
    }
    void clear_group_data(entt::registry &registry)
    {
      auto vbos =
          registry.view<VertexBuffer>().raw();
      auto vertex_data = registry.view<VertexData>();
      if (vertex_data.size() > 0)
      {
        /*  glBindBuffer(GL_ARRAY_BUFFER, vbos[0].id); */
        /*  glMapBufferRange(GL_ARRAY_BUFFER, 0, vertex_data.size() * (sizeof(VertexData)),
                         GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_INVALIDATE_RANGE_BIT); */
        /*        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_data.size(), 0, GL_DYNAMIC_DRAW); */
        /* glMapBufferRange(GL_ARRAY_BUFFER, 0, vertex_data.size() * (sizeof(VertexData)),
                         GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT); */
        /*  glUnmapBuffer(GL_ARRAY_BUFFER); */
      }
    }
    void upload_group_data(entt::registry &registry)
    {
      auto vbos =
          registry.view<VertexBuffer>().raw();
      auto vertex_data = registry.view<VertexData>();
      if (vertex_data.size() > 0)
      {
        auto data = vertex_data.raw();
        GLint size = 0;
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0].id);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        if (size > 0)
        {

          glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_data.size() * (sizeof(VertexData)),
                          data);
        }
        else
        {
          glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * (sizeof(VertexData)),
                       data, GL_STREAM_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
      }
    } // namespace
    void create_vertex_array(entt::registry &registry)
    {
      unsigned int groupVao;
      unsigned int groupVbo;
      auto groupVertexArray = registry.create();
      create_vertex_array(registry, groupVao, groupVbo);
      registry.emplace<VertexArray>(groupVertexArray, groupVao);
      registry.emplace<VertexBuffer>(groupVertexArray, groupVbo);
      registry.emplace<entt::tag<vao_tag>>(groupVertexArray);
      registry.emplace<entt::tag<vbo_tag>>(groupVertexArray);
    }
    void update_sprites(entt::registry &registry)
    {
      if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
      {
        std::set<int> renderGroupToUpdate;
        auto toRender = registry.group<RenderSprite, Position, Transform>();
        for (auto [spriteEntity, renderSprite, position, transform] :
             toRender.each())
        {
          auto &groupRegistry =
              *ptr->render_group_registry[renderSprite.group.index].get();
          std::vector<Graphic::VertexData> data{
              Graphic::VertexData{glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
              Graphic::VertexData{glm::vec4(1.0f, 0.0f, 1.0f, 0.0f)},
              Graphic::VertexData{glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)},
              Graphic::VertexData{glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
              Graphic::VertexData{glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)},
              Graphic::VertexData{glm::vec4(1.0f, 0.0f, 1.0f, 0.0f)}};
          for (int y = 0; y < data.size(); y++)
          {
            auto vertex = groupRegistry.create();
            glm::vec2 uv = Graphic::calculate_uv(renderSprite.sprite, data[y]);
            // model
            glm::mat4 model = Graphic::create_model_matrix(position, transform);
            groupRegistry.emplace<Graphic::VertexData>(vertex, data[y].vertice, uv,
                                                       model, renderSprite.color, spriteEntity);
          }
          renderGroupToUpdate.insert(renderSprite.group.index);
          registry.remove<RenderSprite>(spriteEntity);
          registry.emplace<RenderGroupHandle>(spriteEntity, renderSprite.group);
        }
        for (auto groupIndex : renderGroupToUpdate)
        {
          auto &groupRegistry = *ptr->render_group_registry[groupIndex].get();
          upload_group_data(groupRegistry);
        }
        return;
      }
      handle_error_context();
    }
    void destroy_sprites(entt::registry &registry)
    {
      auto view = registry.view<Destroy, RenderGroupHandle>();
      for (auto entity : view)
      {
        destroy(registry, entity);
      }
    }
    void update_vertices(entt::registry &registry)
    {
      if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
      {
        std::set<int> renderGroupToUpdate;
        auto &observer = *ptr->position_observer.get();

        // position updated
        for (auto entity : observer)
        {
          auto renderGroup = registry.get<RenderGroupHandle>(entity);
          auto transform = registry.get<Transform>(entity);
          auto position = registry.get<Position>(entity);
          auto &groupRegistry =
              *ptr->render_group_registry[renderGroup.index].get();
          auto verticesData = groupRegistry.view<VertexData>();
          for (auto vEntity : verticesData)
          {
            groupRegistry.patch<VertexData>(vEntity, [entity,
                                                      transform,
                                                      position](VertexData &vData) {
              if (vData.sprite == entity)
              {
                glm::mat4 model = Graphic::create_model_matrix(position, transform);
                vData.transform = model;
              }
            });
          }

          renderGroupToUpdate.insert(renderGroup.index);
        }
        for (auto groupIndex : renderGroupToUpdate)
        {
          auto &groupRegistry = *ptr->render_group_registry[groupIndex].get();
          upload_group_data(groupRegistry);
        }
        return;
      }
      handle_error_context();
    }
  } // namespace
  entt::resource_handle<Graphic::Shader>
  Graphic::load_shader(const entt::hashed_string key, ShaderSource &source)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto shader = ptr->shader_cache.load<Graphic::shader_loader>(key, source);
      return shader;
    }
    handle_error_context();
  }
  entt::resource_handle<Graphic::ShaderProgam>
  Graphic::load_shader_program(const entt::hashed_string key,
                               std::vector<Graphic::Shader> &shaders)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto shader =
          ptr->program_cache.load<Graphic::shader_program_loader>(key, shaders);
      // create a entity for the shader
      auto shader_entity = graphicRegistry.create();
      graphicRegistry.emplace<Graphic::ShaderProgam>(shader_entity,
                                                     shader.get().id);
      return shader;
    }
    handle_error_context();
  }
  entt::resource_handle<Graphic::ShaderSource>
  Graphic::load_shader_source(const entt::hashed_string key,
                              const std::string &path,
                              const Graphic::ShaderType type)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto source = ptr->shader_source_cache.load<Graphic::shader_source_loader>(
          key, path, type);
      return source;
    }
    handle_error_context();
  }
  entt::resource_handle<Graphic::Image>
  Graphic::load_image(const entt::hashed_string key, const std::string &path,
                      bool flipVertically)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto image =
          ptr->image_cache.load<Graphic::image_loader>(key, path, flipVertically);
      return image;
    }
    handle_error_context();
  }
  entt::resource_handle<Graphic::Texture>
  Graphic::load_texture(const entt::hashed_string key, Image &image)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto &texture =
          ptr->texture_cache.load<Graphic::texture_image_loader>(key, image);
      return texture;
    }
    handle_error_context();
  }
  entt::resource_handle<Graphic::ShaderProgam> get_shader_program(const entt::hashed_string key)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto &shaderProgam = ptr->program_cache.handle(key);
      return shaderProgam;
    }
    handle_error_context();
  }
  entt::resource_handle<Graphic::Texture> get_texture(const entt::hashed_string key)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto &texture = ptr->texture_cache.handle(key);
      return texture;
    }
    handle_error_context();
  }
  RenderGroupHandle
  Graphic::create_render_group(const entt::hashed_string &shader,
                               const entt::hashed_string &texture)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto &v_render_group_registry = ptr->render_group_registry;
      v_render_group_registry.push_back(
          std::unique_ptr<entt::registry>(new entt::registry()));
      auto render_group_registry = v_render_group_registry.back().get();
      auto entity = render_group_registry->create();
      render_group_registry->emplace<RenderGroup>(entity, shader, texture);
      create_vertex_array(*render_group_registry);
      return RenderGroupHandle{static_cast<int>(v_render_group_registry.size()) -
                               1};
    }
    handle_error_context();
  }
  void Graphic::clear_render_group(RenderGroupHandle group)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto &registry = *ptr->render_group_registry[group.index].get();
      registry.clear<VertexData>();
      return;
    }
    handle_error_context();
  }
  void Graphic::render_group(entt::registry &registry, const RenderGroup &renderGroup)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      auto cameras = graphicRegistry.view<ProjectionMatrix>();
      auto vertices = registry.view<VertexData>();
      auto &shader = ptr->program_cache.handle(renderGroup.shader).get();
      auto &texture = ptr->texture_cache.handle(renderGroup.texture).get();
      auto projectionUniformLocation =
          glGetUniformLocation(shader.id, "projection");
      auto imageLocation = glGetUniformLocation(shader.id, "image");
      auto vaos =
          registry.view<VertexArray>().raw();
      if (vertices.size() > 0)
      {
        glUseProgram(shader.id);
        for (auto entity : cameras)
        {
          auto matrix = cameras.get<ProjectionMatrix>(entity);
          glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE,
                             glm::value_ptr(matrix.value));
          glActiveTexture(GL_TEXTURE0);
          glBindTexture(GL_TEXTURE_2D, texture.id);
          glUniform1i(imageLocation, 0);
          glBindVertexArray(vaos[0].id);
          glDrawArrays(GL_TRIANGLES, 0, vertices.size());

          glUseProgram(0);
          glBindVertexArray(0);
          glBindTexture(GL_TEXTURE_2D, 0);
        }
      }
      return;
    }
    handle_error_context();
  }
  void Graphic::render_group(entt::basic_handle<entt::entity> &group)
  {
    auto test = group.registry();
    auto test2 = group.get<RenderGroup>();
    render_group(*test, test2);
  }
  void Graphic::add_projection_matrix(glm::mat4 projection)
  {
    auto camera = graphicRegistry.create();
    graphicRegistry.emplace<Graphic::ProjectionMatrix>(camera, projection);
  }
  ProjectionMatrix projection_matrix()
  {
    auto e = graphicRegistry.view<Graphic::ProjectionMatrix>().front();
    return graphicRegistry.get<Graphic::ProjectionMatrix>(e);
  }
  void Graphic::init(entt::registry &registry)
  {
    init_context(registry);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void Graphic::update(entt::registry &registry, float dt)
  {

    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      destroy_sprites(registry);
      update_sprites(registry);
      update_vertices(registry);
      auto &observer = *ptr->position_observer.get();

      // position updated
      for (auto entity : observer)
      {
        auto renderGroup = registry.get<RenderGroupHandle>(entity);
        auto &groupRegistry = *ptr->render_group_registry[renderGroup.index].get();
        clear_group_data(groupRegistry);
      }
      return;
    }
    handle_error_context();
  }
  void Graphic::render(entt::registry &registry)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      for (const auto &renderGroupRegistry : ptr->render_group_registry)
      {
        for (auto [entity, groupComponent] :
             renderGroupRegistry->view<RenderGroup>().each())
        {
          render_group(*renderGroupRegistry, groupComponent);
        }
      }
      return;
    }
    handle_error_context();
  }
  void Graphic::destroy(entt::registry &registry, entt::entity entity)
  {
    if (auto *ptr = graphicRegistry.try_ctx<RenderContext>(); ptr)
    {
      std::set<int> renderGroupToUpdate;
      if (registry.has<RenderGroupHandle>(entity))
      {
        auto renderGroup = registry.get<RenderGroupHandle>(entity);
        auto transform = registry.get<Transform>(entity);
        auto position = registry.get<Position>(entity);
        auto &groupRegistry =
            *ptr->render_group_registry[renderGroup.index].get();

        auto &observer = *ptr->position_observer.get();

        auto verticesData = groupRegistry.view<VertexData>();
        for (auto vEntity : verticesData)
        {
          auto vData = groupRegistry.get<VertexData>(vEntity);
          if (vData.sprite == entity)
          {
            groupRegistry.destroy(vEntity);
            renderGroupToUpdate.insert(renderGroup.index);
          }
        }
        for (auto groupIndex : renderGroupToUpdate)
        {
          auto &groupRegistry = *ptr->render_group_registry[groupIndex].get();
          upload_group_data(groupRegistry);
        }
        registry.remove_if_exists<Sprite, RenderGroupHandle, Position>(entity);
      }

      return;
    }
    handle_error_context();
  }
} // namespace Graphic