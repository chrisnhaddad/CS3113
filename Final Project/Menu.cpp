/**
* Author: Christopher Haddad
* Assignment: Pokemon - Final Project
* Date due: May 2, 2:00pm
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
    m_game_state.next_scene_id = -1;

    m_game_state.player = new Entity();

    m_game_state.player->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
}

void Menu::update(float delta_time) {

}

void Menu::render(ShaderProgram* program) {
    Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
        "Final Project - Pokemon", 0.3f, 0.1f, glm::vec3(-4.4, 1.0f, 0.0f));

    Utility::draw_text(program, Utility::load_texture("assets/font1.png"),
        "Press Enter to Start", 0.3f, 0.1f, glm::vec3(-4.0f, 0.0f, 0.0f));
}
