/**
* Author: Christopher Haddad
* Assignment: Pokemon - Final Project
* Date due: May 2, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "LevelD.h"
#include "Menu.h"
#include <string>

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640 * 1.5,
          WINDOW_HEIGHT = 480 * 1.5;

constexpr float BG_RED = 0.01f,
BG_BLUE = 0.55f,
BG_GREEN = 0.95f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
LevelA* g_levelA;
LevelB* g_levelB;
LevelC* g_levelC;
LevelD* g_levelD;
Menu* g_menu;

Scene* g_levels[5];

int g_lives = 3;
bool g_hit = false;
float g_safe_time = 0.0f;

bool g_win = false;
bool g_lose = false;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

Mix_Chunk* g_scene_switch_sfx = nullptr;

void switch_to_scene(Scene* scene) {
    // 1) snapshot the old scene’s party into the shared Entity::party
    if (g_current_scene) {
        Entity::party = g_current_scene->get_state().party;
    }

    Mix_PlayChannel(-1, g_scene_switch_sfx, 0);

    // 2) switch the current scene pointer and load appropriate shaders
    g_current_scene = scene;

    if (scene == g_levelC) {
        g_shader_program.load(
            "shaders/vertex_lit.glsl",
            "shaders/fragment_lit.glsl"
        );
    }
    else {
        g_shader_program.load(
            V_SHADER_PATH,
            F_SHADER_PATH
        );
    }

    // re‑apply camera/projection matrices
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    // 3) initialize the new scene (this will reset its own party)
    scene->initialise();

    // 4) restore the shared party back into the new scene’s GameState
    scene->get_state().party = Entity::party;
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Final Project",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_scene_switch_sfx = Mix_LoadWAV("assets/Item2A.wav");
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    
    // ————— LEVEL SETUP ————— //
    g_levelA = new LevelA();
    g_levelB = new LevelB();
	g_levelC = new LevelC();
    g_levelD = new LevelD();
	g_menu = new Menu();

    g_levels[0] = g_menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
	g_levels[3] = g_levelC;
	g_levels[4] = g_levelD;

    switch_to_scene(g_levels[0]);
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));

    Entity* player = g_current_scene->get_state().player;
    auto& gs = g_current_scene->get_state();
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_app_status = TERMINATED;
                        break;
                        
                    case SDLK_SPACE:
                        // ————— JUMPING ————— //
                        if (g_current_scene->get_state().player->get_collided_bottom())
                        {
                            g_current_scene->get_state().player->jump();
                            Mix_PlayChannel(-1,  g_current_scene->get_state().jump_sfx, 0);
                        }
                         break;

                    case SDLK_z :

                        if (g_current_scene == g_levelA) {
                            Entity* starters = g_current_scene->get_state().starters;

                            if (player->check_collision(gs.treecko)) {
                                g_current_scene->get_state().chosen_starter = "Treecko";
                                gs.party.push_back(gs.treecko);
                                gs.treecko->set_caught();
                                Mix_PlayChannel(-1, g_current_scene->get_state().catch_sfx, 0);
                            }
                            else if (player->check_collision(gs.chimchar)) {
                                g_current_scene->get_state().chosen_starter = "Chimchar";
                                gs.party.push_back(gs.chimchar);
                                gs.chimchar->set_caught();
                                Mix_PlayChannel(-1, g_current_scene->get_state().catch_sfx, 0);
                            }
                            else if (player->check_collision(gs.froakie)) {
                                g_current_scene->get_state().chosen_starter = "Froakie";
                                gs.party.push_back(gs.froakie);
                                gs.froakie->set_caught();
                                Mix_PlayChannel(-1, g_current_scene->get_state().catch_sfx, 0);
                            }
                        }
                        else {
                            for (Entity* poke : gs.catchables) {
                                if (player->check_collision(poke)) {
                                    gs.party.push_back(poke);
                                    poke->set_caught();
                                    Mix_PlayChannel(-1, g_current_scene->get_state().catch_sfx, 0);
                                }
                            }
                        }
                        break;
                    
                    

					case SDLK_RETURN:
						// ————— START GAME ————— //
						if (g_current_scene == g_menu) {
                            Entity::party.clear();
                            g_levelA->get_state().party.clear();
                            g_levelA->get_state().chosen_starter.clear();
                            switch_to_scene(g_levelA);
						}
						break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
    else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
    else if (key_state[SDL_SCANCODE_UP])     g_current_scene->get_state().player->move_up();
    else if (key_state[SDL_SCANCODE_DOWN])   g_current_scene->get_state().player->move_down();

    if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
 
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

	if (g_lose || g_win) {
		return;
	}
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        
        for (int i = 0; i < g_current_scene->get_number_of_enemies(); i++) {
            if (g_current_scene->get_state().player->check_collision(&g_current_scene->get_state().enemies[i]) && !g_hit) {
                Mix_PlayChannel(-1, g_current_scene->get_state().tap_sfx, 0);
                g_lives-=1;
                g_hit = true;
                g_safe_time = 1.0f;

                if (g_lives <= 0) {
                    Mix_PlayChannel(-1, g_current_scene->get_state().lose_sfx, 0);
                    g_lose = true;
                }
            }
        }

        if (g_hit) {
            g_safe_time -= FIXED_TIMESTEP;
            if (g_safe_time <= 0.0f) {
                g_safe_time = 0.0f;
                g_hit = false;
            }
        }

        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);

    glm::vec3 player_position = g_current_scene->get_state().player->get_position();

    g_view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-player_position.x, -player_position.y, 0.0f));

    if ((g_current_scene == g_levelA) && (g_current_scene->get_state().player->get_position().y > -1.0f) && !(g_current_scene->get_state().chosen_starter).empty()) switch_to_scene(g_levelB);
    if (g_current_scene == g_levelB && g_current_scene->get_state().player->get_position().y > 0.0f) switch_to_scene(g_levelC);
    if (g_current_scene == g_levelC && g_current_scene->get_state().player->get_position().x < 0.0f) switch_to_scene(g_levelD);

    if (g_current_scene == g_levelD) {
        auto& gs = g_current_scene->get_state();
        float py = gs.player->get_position().y;
        if (py > -9.0f && gs.party.size() < 5) {
            gs.party.clear();
            Entity::party.clear();
            switch_to_scene(g_levelA);
        }
    }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);

    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);

    g_shader_program.set_view_matrix(glm::mat4(1.0f)); // Reset view

    std::string starter_text = g_current_scene->get_state().chosen_starter;

    if (!starter_text.empty()) {
        std::string message = "You chose " + starter_text+ "!";
        GLuint font_texture = Utility::load_texture("assets/font1.png");

        Utility::draw_text(
            &g_shader_program,
            font_texture,
            message,
            0.4f,     // font size
            0.05f,    // spacing between letters
            glm::vec3(-3.5f, -2.0f, 0.0f)  // position on screen
        );
    }

    if (g_lose) {
        Utility::draw_text(&g_shader_program, Utility::load_texture("assets/font1.png"), "You Lose!", 0.5f, 0.1f, glm::vec3(-2.0f, 0.0f, 0));
    }

    if (g_win) {
        Utility::draw_text(&g_shader_program, Utility::load_texture("assets/font1.png"), "You Win!", 0.5f, 0.1f, glm::vec3(-2.0f, 0.0f, 0));
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{    
    Mix_FreeChunk(g_scene_switch_sfx);
    SDL_Quit();
    
    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_levelA;
    delete g_levelB;
	delete g_levelC;
	delete g_menu;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
