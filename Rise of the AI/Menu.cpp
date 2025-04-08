/**
* Author: Christopher Haddad
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

Menu::~Menu(){}

void Menu::initialise() {
    m_game_state.next_scene_id = -1; // Stay on menu until user starts game

    m_game_state.player = new Entity();

    m_game_state.player->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Menu::update(float delta_time) {

}

void Menu::render(ShaderProgram* program) {
    Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
        "Rise of the AI", 0.5f, 0.2f, glm::vec3(0.5f, -3.0f, 0.0f));

    Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
        "Press Enter to Start", 0.3f, 0.1f, glm::vec3(1.0f, -5.0f, 0.0f));
}
