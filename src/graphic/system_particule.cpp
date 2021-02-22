
#include "system_particule.hpp"
#include "graphic/helper_graphic_pipeline.hpp"
#include "graphic/system_graphic_pipeline.hpp"
#include "gameplay/component_gameplay.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Graphic
{
    namespace Particule
    {
        struct ParticuleVertexData
        {
            glm::vec2 position;
            glm::vec2 textCoords;
        };

        unsigned int particuleVao;
        unsigned int particuleVbo;
        const entt::hashed_string shader_key = "particle"_hs;

        void update_particule_life(entt::registry &registry)
        {
            auto particules = registry.view<Particle>(entt::exclude_t<Destroy>());
            for (auto [entity, particule] : particules.each())
            {
            }
        }
        entt::entity first_unused_particule(entt::registry &registry)
        {
            auto lastDestroyed = registry.view<Particle, Destroy>().front();
            if (lastDestroyed != entt::null)
            {
                return lastDestroyed;
            }
            return registry.view<Particle>().front();
        }
        void respawn_particule(entt::registry &registry,
                               Particle &particule,
                               const Position &position,
                               const Gameplay::Velocity &velocity,
                               glm::vec2 offset)
        {
            float random = ((rand() % 100) - 50) / 10.0f;
            float rColor = 0.5f + ((rand() % 100) / 100.0f);
            particule.position = glm::vec2(position.value) + random + offset;
            particule.color = glm::vec4(rColor, rColor, rColor, 1.0f);
            particule.life = 1.0f;
            particule.velocity = velocity.value * 0.1f;
        }

        void init(entt::registry &registry)
        {

            auto texture = Graphic::get_texture("atlas"_hs).get();
            auto sprite = Graphic::Sprite({texture, glm::vec4(0, 128, 500, 500)});
            auto particle_vs_source = Graphic::load_shader_source(
                "particle_vs"_hs, "./shader/particule.vect", Graphic::VertexShader);
            auto particle_fs_source = Graphic::load_shader_source(
                "particle_fs"_hs, "./shader/particule.frag", Graphic::FragmentShader);

            auto particle_vs = Graphic::load_shader("particle_vs"_hs, particle_vs_source);
            auto particle_fs = Graphic::load_shader("particle_fs"_hs, particle_fs_source);

            auto particle_shader_r = Graphic::load_shader_program(
                shader_key, std::vector<Graphic::Shader>{particle_vs, particle_fs});

            glm::vec2 particle_quad[] = {
                glm::vec2(0.0f, 1.0f),
                Graphic::calculate_uv(sprite, glm::vec2(0.0f, 1.0f)),
                glm::vec2(1.0f, 0.0f),
                Graphic::calculate_uv(sprite, glm::vec2(1.0f, 0.0f)),
                glm::vec2(0.0f, 0.0f),
                Graphic::calculate_uv(sprite, glm::vec2(0.0f, 0.0f)),

                glm::vec2(0.0f, 1.0f),
                Graphic::calculate_uv(sprite, glm::vec2(0.0f, 1.0f)),
                glm::vec2(1.0f, 1.0f),
                Graphic::calculate_uv(sprite, glm::vec2(1.0f, 1.0f)),
                glm::vec2(1.0f, 0.0f),
                Graphic::calculate_uv(sprite, glm::vec2(1.0f, 0.0f)),
            };

            glGenVertexArrays(1, &particuleVao);
            glBindVertexArray(particuleVao);

            // fill mesh buffer
            glGenBuffers(1, &particuleVbo);

            glBindBuffer(GL_ARRAY_BUFFER, particuleVbo);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                                  (void *)0);

            glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_DYNAMIC_DRAW);

            // vertex
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            unsigned int nr_particle = 500;
            for (unsigned int i = 0; i < nr_particle; i++)
            {
                auto particleEntity = registry.create();
                auto particle = registry.emplace<Particle>(particleEntity);
                registry.emplace<Sprite>(particleEntity, sprite);
                registry.emplace<Destroy>(particleEntity);
            }
        }
        void render(entt::registry &registry)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            auto shader = Graphic::get_shader_program(shader_key).get();

            auto particles = registry.view<Particle, Sprite>(entt::exclude_t<Destroy>());
            auto projection = Graphic::projection_matrix();
            auto offsetPosition = glGetUniformLocation(shader.id, "offset");
            auto colorPosition = glGetUniformLocation(shader.id, "color");
            auto imageLocation = glGetUniformLocation(shader.id, "sprite");
            auto projectionLocation = glGetUniformLocation(shader.id, "projection");
            glUseProgram(shader.id);
            glUniformMatrix4fv(projectionLocation, 1, GL_FALSE,
                               glm::value_ptr(projection.value));
            for (auto [entity, particle, sprite] : particles.each())
            {
                if (particle.life > 0)
                {

                    glUniform2fv(offsetPosition, 1, glm::value_ptr(particle.position));
                    glUniform4fv(colorPosition, 1,
                                 glm::value_ptr(particle.color));

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, sprite.texture.id);
                    glUniform1i(imageLocation, 0);
                    glBindVertexArray(particuleVao);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
            glBindVertexArray(0);
            glUseProgram(0);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        void update(entt::registry &registry, float dt)
        {

            auto allParticles = registry.view<Particle, Target>();
            auto positions = registry.view<Position>();
            auto velocities = registry.view<Gameplay::Velocity>();
            unsigned int nr_new_particles = 2;
            // add new particles
            for (unsigned int i = 0; i < nr_new_particles; ++i)
            {
                auto unusedParticleEntity = first_unused_particule(registry);
                auto &particule = allParticles.get<Particle>(unusedParticleEntity);
                auto target = registry.get<Target>(unusedParticleEntity).target;
                if (target != entt::null)
                {
                    if (registry.all_of<Position, Graphic::Transform, Gameplay::Velocity>(target))
                    {
                        const auto [position] = positions.get(target);
                        const auto [velocity] = velocities.get(target);
                        auto targetTransform = registry.get<Transform>(target);
                        respawn_particule(registry, particule, position, velocity, targetTransform.size / 2.f);
                        registry.remove<Destroy>(unusedParticleEntity);
                    }
                }
            }
            auto particles = registry.view<Particle, Target>(entt::exclude_t<Destroy>());
            for (auto [entity, particle, target] : particles.each())
            {
                particle.life -= dt;
                if (particle.life > 0)
                {
                    particle.color.a -= dt * 2.5f;
                    particle.position -= particle.velocity * dt;
                }
                else
                {
                    registry.emplace<Destroy>(entity);
                }
            }
        }

    } // namespace Particule

} // namespace Graphic