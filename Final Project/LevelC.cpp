/**
* Author: Christopher Haddad
* Assignment: Pokemon - Final Project
* Date due: May 2, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 23
#define LEVEL_HEIGHT 25

constexpr float BG_RED = 0.7f,
BG_BLUE = 0.41f,
BG_GREEN = 0.12f,
BG_OPACITY = 1.0f;

constexpr char SPRITESHEET_FILEPATH[] = "assets/character.png",
ENEMY_FILEPATH[] = "assets/slime.png";

unsigned int LEVELC_DATA[] =
{
   71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
   71, 74, 81, 81, 81, 73, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71,
   71, 72,  0,  0,  0, 70, 71, 74, 81, 81, 81, 81, 81, 81, 81, 81, 81, 73, 71, 71, 71, 71, 71,
   71, 72,  0,  0,  0, 70, 71, 72,  0,  0,  0,  0,  0,  0,  0,  0,  0, 70, 71, 71, 71, 71, 71,
   71, 72,  0,  0,  0, 70, 71, 72,  0,  0,  0,  0,  0,  0,  0,  0,  0, 80, 81, 81, 81, 73, 71,
   71, 72,  0,  0,  0, 80, 81, 82,  0,  0,  0,  0, 60, 61, 62,  0,  0,  0,  0,  0,  0, 70, 71,
   71, 72,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 70, 71, 72,  0,  0,  0,  0,  0,  0, 70, 71,
   71, 72,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 80, 81, 82,  0,  0,  0,  0,  0,  0, 70, 71,
   71, 72,  0,  0, 60, 61, 61, 61, 61, 62,  0,  0,  0,  0,  0,  0,  0, 60, 61, 61, 61, 63, 71,
   71, 72,  0,  0, 70, 71, 71, 71, 71, 72,  0,  0,  0,  0,  0,  0,  0, 70, 71, 71, 71, 71, 71,
   71, 72,  0,  0, 70, 74, 81, 81, 81, 82,  0,  0,  0,  0,  0,  0,  0, 80, 81, 81, 81, 73, 71,
   71, 72,  0,  0, 70, 72,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 70, 71,
   71, 72,  0,  0, 70, 72,  0,  0,  0,  0,  0,  0,  0, 60, 61, 61, 62,  0,  0,  0,  0, 70, 71,
   71, 72,  0,  0, 70, 72,  0,  0,  0,  0,  0,  0,  0, 70, 71, 71, 64, 61, 62,  0,  0, 70, 71,
   71, 72,  0,  0, 70, 64, 61, 61, 61, 62,  0,  0,  0, 70, 71, 71, 71, 71, 72,  0,  0, 70, 71,
   71, 72,  0,  0, 70, 71, 71, 71, 71, 72,  0,  0,  0, 70, 71, 71, 71, 71, 72,  0,  0, 70, 71,
   71, 72,  0,  0, 80, 81, 81, 73, 71, 72,  0,  0,  0, 70, 71, 74,  81,81, 82,  0,  0, 70, 71,
   71, 72,  0,  0,  0,  0,  0, 70, 71, 72,  0,  0,  0, 80, 81, 82,  0,  0,  0,  0,  0, 70, 71,
   81, 82,  0,  0,  0,  0,  0, 70, 71, 72,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 70, 71,
    0,  0,  0,  0,  0,  0,  0, 70, 71, 72,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 70, 71,
    0,  0,  0,  0,  0,  0,  0, 70, 71, 64, 61, 61, 61, 61, 61, 61, 61, 61, 62,  0,  0, 70, 71,
   61, 61, 61, 61, 61, 61, 61, 63, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 72,  0,  0, 70, 71,
   71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 72,  0,  0, 70, 71,
   71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 64, 61, 61, 63, 71,
   71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71, 71
};

LevelC::~LevelC()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.tap_sfx);
    Mix_FreeChunk(m_game_state.win_sfx);
    Mix_FreeChunk(m_game_state.lose_sfx);

    Mix_FreeChunk(m_game_state.catch_sfx);
}

void LevelC::initialise()
{
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 10, 12);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    int player_walking_animation[4][4] =
    {
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 9, 10, 11 },
        { 12, 13, 14, 15 }
    };

    int gliscor_walk[4][4] =
    {
        { 4, 6},
        { 1, 3},
        { 5, 7},
        { 0, 2}
    };

    int garchomp_walk[4][4] =
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

    m_game_state.player->set_position(glm::vec3(20.0f, -22.0f, 0.0f));
    m_game_state.player->face_up();

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);

    GLuint gliscor_texture = Utility::load_texture("assets/gliscor.png");
    m_game_state.gliscor = new Entity(
        gliscor_texture,
        1.0f,
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        gliscor_walk,
        0.0f,
        2,
        0,
        2,
        4,
        1.0f,
        1.0f,
        PLAYER
    );
    m_game_state.gliscor->set_position(glm::vec3(15.0f, -3.0f, 0.0f));

    GLuint garchomp_texture = Utility::load_texture("assets/garchomp.png");
    m_game_state.garchomp = new Entity(
        garchomp_texture,
        1.0f,
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        garchomp_walk,
        0.0f,
        2,
        0,
        2,
        4,
        1.0f,
        1.0f,
        PLAYER
    );
    m_game_state.garchomp->set_position(glm::vec3(4.0f, -2.0f, 0.0f));

    m_game_state.catchables.push_back(m_game_state.gliscor);
    m_game_state.catchables.push_back(m_game_state.garchomp);


    GLuint dirt_texture = Utility::load_texture("assets/dirt.png");
    m_game_state.dirt = new Entity(dirt_texture, 1.0f, 50.0f, 50.0f, PLATFORM);
    m_game_state.dirt->set_position(glm::vec3(10.0f, -10.0f, 0.0f));

    /**
    Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    m_number_of_enemies = ENEMY_COUNT;


    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/caketown.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(25.0f);

    m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");
    m_game_state.tap_sfx = Mix_LoadWAV("assets/tap.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/win.wav");
    m_game_state.lose_sfx = Mix_LoadWAV("assets/lose.wav");

    m_game_state.catch_sfx = Mix_LoadWAV("assets/catch.wav");
}

void LevelC::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    m_game_state.dirt->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );


    if (m_game_state.gliscor->get_position().x >= 15.0f) {
        m_game_state.gliscor->move_left();
    }
    else if (m_game_state.gliscor->get_position().x <= 8.0f) {
        m_game_state.gliscor->move_right();
    }

    m_game_state.gliscor->update(delta_time,
        m_game_state.player, nullptr, 0, m_game_state.map);


    glm::vec3 g = m_game_state.garchomp->get_position();

    if (g.y >= -2.0f && g.x > 2.0f)
    {
        m_game_state.garchomp->move_left();
    }
    else if (g.x <= 2.0f && g.y > -4.0f)
    {
        m_game_state.garchomp->move_down();
    }
    else if (g.y <= -4.0f && g.x < 4.0f)
    {
        m_game_state.garchomp->move_right();
    }
    else if (g.x >= 4.0f && g.y < -2.0f) 
    {
        m_game_state.garchomp->move_up();
    }

    m_game_state.garchomp->update(delta_time,
        m_game_state.player, nullptr, 0, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
}

void LevelC::render(ShaderProgram* g_shader_program)
{
    m_game_state.dirt->render(g_shader_program);
    m_game_state.map->render(g_shader_program);

    if (!m_game_state.gliscor->get_caught()) {
        m_game_state.gliscor->render(g_shader_program);
    }
    if (!m_game_state.garchomp->get_caught()) {
        m_game_state.garchomp->render(g_shader_program);
    }

    glm::vec3 p = m_game_state.player->get_position();
    g_shader_program->setLightPosition(p);

    m_game_state.player->render(g_shader_program);


    g_shader_program->set_view_matrix(glm::mat4(1.0f));
    for (int i = 0; i < m_game_state.party.size(); i++) {
        m_game_state.party[i]->set_position(glm::vec3(4.0f, 3.0f - i, 0.0f));
        m_game_state.party[i]->face_down();
        m_game_state.party[i]->render(g_shader_program);
    }

    for (int i = 0; i < ENEMY_COUNT; i++)
        m_game_state.enemies[i].render(g_shader_program);
}