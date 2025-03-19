/**
* Author: Christopher Haddad
* Assignment: Lunar Lander
* Date due: 2025-3-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#ifndef ENTITY_H
#define ENTITY_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"

constexpr float ACC_OF_GRAVITY = -0.5f;


class Entity
{
private:
    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;
    float m_rotation = 0.0f;
    bool m_win = false;
    bool m_lose = false;
    float m_platform_movement = 1.0f;
    
    glm::mat4 m_model_matrix;
    
    // ————— TEXTURES ————— //
    GLuint    m_texture_id;


    int m_width  = 1,
        m_height = 1;

    float m_fuel = 500.0f;

public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 4;

    // ————— METHODS ————— //
    Entity();
    
    Entity(GLuint texture_id);
    ~Entity();

    bool const check_collision(Entity* other) const;

    void update(float delta_time, Entity* collidable_entities, int collidable_entity_count);
    void render(ShaderProgram *program);

    void move_platform(float delta_time);

    // ————— GETTERS ————— //
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     get_rotation()           const { return m_rotation; }
	float     get_fuel()               const { return m_fuel; }
    bool      get_win()                const { return m_win; }
    bool      get_lose()               const { return m_lose; }

    // ————— SETTERS ————— //
    void const set_position(glm::vec3 new_position)     { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity)     { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_scale(glm::vec3 new_scale)           { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id)    { m_texture_id = new_texture_id; }
    void set_rotation(float new_angle)                  { m_rotation = new_angle; }
	void set_fuel(float new_fuel)                       { m_fuel = new_fuel; }

};

#endif // ENTITY_H
