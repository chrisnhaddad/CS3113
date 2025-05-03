/**
* Author: Christopher Haddad
* Assignment: Pokemon - Final Project
* Date due: May 2, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 21
#define LEVEL_HEIGHT 17

constexpr float BG_RED = 0.11f,
BG_BLUE = 0.51f,
BG_GREEN = 0.22f,
BG_OPACITY = 0.05f;

constexpr char SPRITESHEET_FILEPATH[] = "assets/character.png",
ENEMY_FILEPATH[] = "assets/slime.png";

unsigned int LEVELB_DATA[] =
{
    88, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 87,  0, 0, 88,
    88,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 88,
    88,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 88,
    88,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 88,
    88,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 88,
    21, 22, 23, 24, 25,  0,  0,  0,  0,  0,  0, 60, 61, 61, 61, 61, 61, 62,  0, 0, 88,
    31, 32, 33, 34, 35,  0, 75, 76,  0,  0,  0, 70, 92, 93, 93, 93, 94, 72,  0, 0, 88,
    41, 42, 43, 44, 45,  0,  0,  0,  0,  0,  0, 70,112,113,113,103,104, 72,  0, 0, 88,
    51, 52, 53, 54, 55,  0,  0,  0,  0,  0,  0, 80, 81, 81, 73,102,104, 72,  0, 0, 88,
    88, 47,  0,  0,  0,  0, 60, 61, 62,  0,  0,  0,  0, 76, 70,102,104, 72,  0, 0, 88,
    88, 57,  0,  0,  0,  0, 70, 71, 72,  0,  0,  0,  0,  0, 70,112,114, 72,  0, 0, 88,
    88, 48,  0,  0,  0,  0, 80, 81, 82,  0,  0, 75,  0,  0, 80, 81, 81, 82,  0, 0, 88,
    88,  7,  8,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 88,
    88, 17, 18, 19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 88,
    88, 27, 28, 29,  0,  0,  0,  0,  0,  0, 75, 76,  0,  0,  0,  0,  0,  0,  0, 0, 88,
    88, 37, 38, 39, 48,  0,  0,  0,  0, 76, 75, 75,  0,  0,  0,  0,  0,  0,  0, 0, 88,
    85, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86,86, 87

};

LevelB::~LevelB()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.tap_sfx);
    Mix_FreeChunk(m_game_state.lose_sfx);
    Mix_FreeChunk(m_game_state.catch_sfx);
}

void LevelB::initialise()
{
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELB_DATA, map_texture_id, 1.0f, 10, 12);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);


    int player_walking_animation[4][4] =
    {
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 9, 10, 11 },
        { 12, 13, 14, 15 }
    };

    int electivire_walk[4][4] =
    {
        { 4, 6},
        { 1, 3},
        { 5, 7},
        { 0, 2}
    };

    int snorlax_walk[4][4] =
    {
        { 4, 6},
        { 1, 3},
        { 5, 7},
        { 0, 2}
    };


    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    m_game_state.player = new Entity(
        player_texture_id,
        5.0f,
        acceleration,
        0.0f,
        player_walking_animation, 
        0.0f,
        4,
        0,
        4,
        4,
        1.0f,
        1.0f,
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(2.0f, -8.5f, 0.0f));
    m_game_state.player->face_down();

    m_game_state.player->set_jumping_power(3.5f);


    GLuint electivire_texture = Utility::load_texture("assets/electivire.png");
    m_game_state.electivire = new Entity(
        electivire_texture,
        1.0f,
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        electivire_walk, 
        0.0f, 
        2,
        0,
        2,
        4,
        1.0f,
        1.0f,
        PLAYER
    );
    m_game_state.electivire->set_position(glm::vec3(5.0f, -1.0f, 0.0f));
    m_game_state.electivire->face_down();


    GLuint snorlax_texture = Utility::load_texture("assets/snorlax.png");
    m_game_state.snorlax = new Entity(
        snorlax_texture,
        1.0f,
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        snorlax_walk,
        0.0f,
        2,
        0,
        2,
        4,
        1.0f,
        1.0f,
        PLAYER
    );
    m_game_state.snorlax->set_position(glm::vec3(18.0f, -3.0f, 0.0f));
    m_game_state.snorlax->face_down();

    m_game_state.catchables.push_back(m_game_state.electivire);
    m_game_state.catchables.push_back(m_game_state.snorlax);

    

    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    m_number_of_enemies = ENEMY_COUNT;

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    }


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/caketown.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(25.0f);

    m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
    m_game_state.tap_sfx = Mix_LoadWAV("assets/tap.wav");
    m_game_state.lose_sfx = Mix_LoadWAV("assets/lose.wav");

    m_game_state.catch_sfx = Mix_LoadWAV("assets/catch.wav");
}

void LevelB::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    glm::vec3 p = m_game_state.electivire->get_position();

    if (p.y >= -1.0f && p.x > 3.0f) 
    {
        m_game_state.electivire->move_left();
    }
    else if (p.x <= 2.0f && p.y > -3.0f)
    {
        m_game_state.electivire->move_down();
    }
    else if (p.y <= -3.0f && p.x < 5.0f)
    {
        m_game_state.electivire->move_right();
    }
    else if (p.x >= 5.0f && p.y < -2.0f) 
    {
        m_game_state.electivire->move_up();
    }

    m_game_state.electivire->update(delta_time,
        m_game_state.player, nullptr, 0, m_game_state.map);

    if (m_game_state.snorlax->get_position().y > -6.0f) {
		m_game_state.snorlax->move_down();
	}
    else if (m_game_state.snorlax->get_position().y < -10.0f){
        m_game_state.snorlax->move_up();
    }

    m_game_state.snorlax->update(delta_time,
        m_game_state.player, nullptr, 0, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
}


void LevelB::render(ShaderProgram* program)
{
    m_game_state.map->render(program);

    if (!m_game_state.electivire->get_caught()) {
        m_game_state.electivire->render(program);
    }
    if (!m_game_state.snorlax->get_caught()) {
        m_game_state.snorlax->render(program);
    }

    m_game_state.player->render(program);

    program->set_view_matrix(glm::mat4(1.0f));
    for (int i = 0; i < m_game_state.party.size(); i++) {
        m_game_state.party[i]->set_position(glm::vec3(4.0f, 3.0f - i, 0.0f));
        m_game_state.party[i]->face_down();
        m_game_state.party[i]->render(program);
    }
}