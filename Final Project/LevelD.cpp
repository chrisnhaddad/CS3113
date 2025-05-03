/**
* Author: Christopher Haddad
* Assignment: Pokemon - Final Project
* Date due: May 2, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelD.h"
#include "Utility.h"

#define LEVEL_WIDTH 18
#define LEVEL_HEIGHT 20

constexpr float BG_RED = 0.01f,
BG_BLUE = 0.55f,
BG_GREEN = 0.95f,
BG_OPACITY = 1.0f;

constexpr char SPRITESHEET_FILEPATH[] = "assets/character.png",
ENEMY_FILEPATH[] = "assets/slime.png";

unsigned int LEVELD_DATA[] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0, 60, 61, 61, 61, 62,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0, 70,  0,  0,  0, 72,  0,  0,  0,  0,  0,  0,  0,
    0, 60, 61, 61, 61, 61, 63,  0,  0,  0, 64, 61, 61, 62, 61, 61, 61, 61,
    0, 70,  0, 75,  0,  0,  0,  0,  0,  0,  0, 79, 79, 70,  0,  0,  0,  0,
    0, 70,  0,  0, 76,  0,  0,  0,  0,  0,  0, 89, 89, 70,  0,  0,  0,  0,
    0, 70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 70,  0,  0,  0,  0,
    0, 70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 70,  0,  0,  0,  0,
    0, 70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 70,  0,  0,  0,  0,
    0, 70,  0, 76,  0, 77,  0,  0,  0,  0, 76,  0,  0, 70,  0,  0,  0,  0,
    0, 70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 80, 81, 81, 81, 81,
    0, 70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0, 70,  0,  0,  0,  0,  0,  0, 76,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0, 70,  0,  0,  0,  0,  0, 77, 77,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0, 80, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0

};

LevelD::~LevelD()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
    Mix_FreeChunk(m_game_state.tap_sfx);
    Mix_FreeChunk(m_game_state.lose_sfx);

    Mix_FreeChunk(m_game_state.catch_sfx);
    Mix_FreeChunk(m_game_state.win_sfx);

}

void LevelD::initialise()
{
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELD_DATA, map_texture_id, 1.0f, 10, 12);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);


    int player_walking_animation[4][4] =
    {
        { 0, 1, 2, 3 },
        { 4, 5, 6, 7 },
        { 8, 9, 10, 11 },
        { 12, 13, 14, 15 }
    };

    static int singleFrame[4][4] = {
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }, 
        { 0, 0, 0, 0 }
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

    m_game_state.player->set_position(glm::vec3(15.0f, -15.0f, 0.0f));
    m_game_state.player->face_left();

    // Jumping
    m_game_state.player->set_jumping_power(3.5f);

    GLuint rayquaza_texture = Utility::load_texture("assets/rayquaza.png");

    m_game_state.rayquaza = new Entity(
        rayquaza_texture,
        1.0f,
        glm::vec3(0),
        0.0f,
        singleFrame,
        0.0f,
        1,
        0,
        1,
        1,
        1.5f, 1.5f,
        PLATFORM
    );
    m_game_state.rayquaza->set_position(glm::vec3(8.0f, -5.0f, 0.0f));
    m_game_state.rayquaza->set_poke_name("Rayquaza");
    m_game_state.catchables.push_back(m_game_state.rayquaza);


    GLuint ground_texture = Utility::load_texture("assets/ground.png");
    m_game_state.ground = new Entity(ground_texture, 1.0f, 22.0f, 10.0f, PLATFORM);
    m_game_state.ground->set_position(glm::vec3(12.0f, -11.5f, 0.0f));

    m_game_state.ground2 = new Entity(ground_texture, 1.0f, 3.0f, 3.0f, PLATFORM);
    m_game_state.ground2->set_position(glm::vec3(8.0f, -6.0f, 0.0f));

    GLuint cyrus_texture = Utility::load_texture("assets/cyrus.png");
    m_game_state.cyrus = new Entity(cyrus_texture, 1.0f, 1.0f, 1.0f, PLATFORM);
    m_game_state.cyrus->set_position(glm::vec3(8.0f, -8.0f, 0.0f));


    GLuint win_texture = Utility::load_texture("assets/win_text.png");
    m_game_state.textbox_win = new Entity(win_texture, 0.0f, 5.0f, 2.0f, PLATFORM);
    m_game_state.textbox_win->set_position(glm::vec3(9.0f, -7.0f, 0.0f));

    GLuint lose_texture = Utility::load_texture("assets/lose_text.png");
    m_game_state.textbox_lose = new Entity(lose_texture, 0.0f, 5.0f, 2.0f, PLATFORM);
    m_game_state.textbox_lose->set_position(glm::vec3(9.0f, -7.0f, 0.0f));

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
    m_game_state.win_sfx = Mix_LoadWAV("assets/chipquest.wav");
}

void LevelD::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, nullptr, 0, m_game_state.map);

    m_game_state.ground->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );

    m_game_state.ground2->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );

    m_game_state.cyrus->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );

    m_game_state.textbox_win->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );

    m_game_state.textbox_lose->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );

    m_game_state.rayquaza->update(
        delta_time,
        m_game_state.player,
        nullptr, 
        0,
        m_game_state.map
    );

    if (!m_game_state.win && m_game_state.party.size() >= 5 && m_game_state.player->get_position().y >= -9.0f) {
        m_game_state.win = true;
    }

    if (!m_game_state.rayquaza_caught
        && m_game_state.party.size() == 6)
    {
        Mix_PlayChannel(-1, m_game_state.win_sfx, 0);
        m_game_state.rayquaza_caught = true;
    }


    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
}

void LevelD::render(ShaderProgram* program)
{
    m_game_state.ground->render(program);
	m_game_state.ground2->render(program);
    m_game_state.map->render(program);

    if (!m_game_state.rayquaza->get_caught()) {
        m_game_state.rayquaza->render(program);
    }

    m_game_state.player->render(program);
    
    if (!m_game_state.win) {
        m_game_state.cyrus->render(program);

        if (m_game_state.party.size() >= 5) {
            m_game_state.textbox_win->render(program);
        }
        else {
            m_game_state.textbox_lose->render(program);
        }
    }

    program->set_view_matrix(glm::mat4(1.0f));
    for (int i = 0; i < m_game_state.party.size(); i++) {
        m_game_state.party[i]->set_position(glm::vec3(4.0f, 3.0f - i, 0.0f));
        m_game_state.party[i]->face_down();
        m_game_state.party[i]->render(program);
    }

    if (m_game_state.rayquaza_caught) {
        static GLuint font_tex = Utility::load_texture("assets/font1.png");
        Utility::draw_text(
            program,
            font_tex,
            "You Win!",
            0.5f,
            0.1f,
            glm::vec3(-2.0f, 0.0f, 0.0f)  
        );
    }
}