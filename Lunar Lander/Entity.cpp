/**
* Author: Christopher Haddad
* Assignment: Lunar Lander
* Date due: 2025-3-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

// Default constructor
Entity::Entity()
    : m_position(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f), m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f) {}

// Parameterized constructor
Entity::Entity(GLuint texture_id)
    : m_position(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f), m_texture_id(texture_id), m_velocity(0.0f),
      m_acceleration(0.0f) {}

Entity::~Entity() { }


bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 3.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 7.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void Entity::move_platform(float FIXED_TIMESTEP) {
    if (m_position.x < -4.5f) {
        m_platform_movement = 1.0f;
    }
    else if (m_position.x > 0.0f) {
        m_platform_movement = -1.0f;
    }

	m_position.x += m_platform_movement * FIXED_TIMESTEP;

    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
}

void Entity::update(float delta_time, Entity* collidable_entities, int collidable_entity_count)
{
    for (int i = 0; i < collidable_entity_count; i++)
    {
        if (check_collision(&collidable_entities[i])) {
            if (m_velocity.y > -1.0f && (m_rotation > -5.0f) && (m_rotation < 5.0f)) {
                m_win = true;
            }
            else {
                m_lose = true;
            }

            return;
        }
    }

    if ((m_position.y < -4.0f) || (m_position.y > 4.0f)) {
        m_lose = true;
    } else if ((m_position.x < -5.0) || (m_position.x > 5.0)) {
        m_lose = true;
    }
    
    m_velocity += m_acceleration * delta_time;
    m_position += m_velocity * delta_time;
    
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
    m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    m_acceleration = glm::vec3(0.0f, ACC_OF_GRAVITY, 0.0f);
}

void Entity::render(ShaderProgram *program)
{
    program->set_model_matrix(m_model_matrix);
    
    float vertices[]   = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = {  0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    
    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
