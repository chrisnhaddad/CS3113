/**
* Author: Christopher Haddad
* Assignment: Pokemon - Final Project
* Date due: May 2, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 8
#define LEVEL_HEIGHT 11

constexpr char SPRITESHEET_FILEPATH[] = "assets/character.png",
ASH_FILEPATH[] = "assets/ash.png",
ENEMY_FILEPATH[] = "assets/slime.png";

unsigned int LEVEL_DATA[] =
{
    0,  0,  88, 86, 86, 88, 0,  0,
    85, 86, 87, 0,  0,  85, 86, 87,
    88, 0 , 0 , 0 , 0 , 0 , 0 , 88,
    88, 0 , 0 , 0 , 0 , 0 , 0 , 88,
    88, 0 , 0 , 0 , 0 , 0 , 0 , 88,
    88, 0 , 0 , 0 , 0 , 0 , 0 , 88,
    98, 0,  0,  0,  0,  0,  0,  98,
    88, 0 , 0 , 0 , 0 , 0 , 0 , 88,
    88, 0 , 0 , 0 , 0 , 0 , 0 , 88,
    88, 0 , 0 , 0 , 0 , 0 , 0 , 88,
    85, 86, 86, 86, 86, 86, 86, 87

};

LevelA::~LevelA()
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

void LevelA::initialise()
{
    m_game_state.next_scene_id = -1;

    GLuint map_texture_id = Utility::load_texture("assets/tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 10, 12);

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

    GLuint treecko_texture = Utility::load_texture("assets/treecko.png");
    GLuint chimchar_texture = Utility::load_texture("assets/chimchar.png");
    GLuint froakie_texture = Utility::load_texture("assets/froakie.png");

    m_game_state.treecko = new Entity(
        treecko_texture, 
        1.0f, 
        glm::vec3(0),
        0.0f,
        singleFrame,
        0.0f,
        1,
        0,
        1,
        1,
        1.0f, 1.0f,
        PLATFORM
    );
    m_game_state.treecko->set_position(glm::vec3(3.5f, -2.0f, 0.0f));
    m_game_state.treecko->set_poke_name("Treecko");

    
    m_game_state.chimchar = new Entity(
        chimchar_texture,
        1.0f,
        glm::vec3(0),
        0.0f,
        singleFrame,
        0.0f,
        1,
        0,
        1,
        1,
        1.0f, 1.0f,
        PLATFORM
    );
    m_game_state.chimchar->set_position(glm::vec3(5.5f, -2.0f, 0.0f));
    m_game_state.chimchar->set_poke_name("Chimchar");


    m_game_state.froakie = new Entity(
        froakie_texture, 
        1.0f,
        glm::vec3(0), 
        0.0f,
        singleFrame, 
        0.0f,
        1,
        0,
        1,
        1,
        1.0f, 1.0f,
        PLATFORM
    );
    m_game_state.froakie->set_position(glm::vec3(1.5f, -2.0f, 0.0f));
    m_game_state.froakie->set_poke_name("Froakie");

    glm::vec3 acceleration = glm::vec3(0.0f);

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

    m_game_state.player->set_position(glm::vec3(3.5f, -8.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(3.0f);

    GLuint ash_texture = Utility::load_texture("assets/ash2.png");
    m_game_state.ash = new Entity(ash_texture, 1.0f, 1.0f, 1.0f, PLATFORM);
    m_game_state.ash->set_position(glm::vec3(3.5f, -5.0f, 0.0f));

    GLuint textbox_texture = Utility::load_texture("assets/ashtextintro.png");
    m_game_state.textbox = new Entity(
        textbox_texture,
        0.0f,
        5.0f,
        2.0f,
        PLATFORM
    );
    m_game_state.textbox->set_position(glm::vec3(5.0f, -4.0f, 0.0f));

    /**
    Enemies' stuff */

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
    m_game_state.lose_sfx = Mix_LoadWAV("assets/lose.wav");

    m_game_state.catch_sfx = Mix_LoadWAV("assets/catch.wav");
}

void LevelA::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.ash, ENEMY_COUNT, m_game_state.map);

    m_game_state.ash->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );

    m_game_state.textbox->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );

    m_game_state.treecko->update(
        delta_time,
        m_game_state.player,
        nullptr,
        0,
        m_game_state.map
    );

    m_game_state.chimchar->update(
        delta_time,
        m_game_state.player,   
        nullptr,               
        0,
        m_game_state.map
    );

    m_game_state.froakie->update(
        delta_time,
        m_game_state.player,
        nullptr,   
        0,
        m_game_state.map
    );
}


void LevelA::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.ash->render(g_shader_program);
    m_game_state.treecko->render(g_shader_program);
    m_game_state.chimchar->render(g_shader_program);
    m_game_state.froakie->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
	m_game_state.textbox->render(g_shader_program);
    
}