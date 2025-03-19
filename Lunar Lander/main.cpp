/**
* Author: Christopher Haddad
* Assignment: Lunar Lander
* Date due: 2025-3-15, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

// main.cpp
#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1

#ifdef _WINDOWS
    #include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 640 * 1.5,
              WINDOW_HEIGHT = 480 * 1.5;

constexpr float BG_RED = 0.1922f,
                BG_BLUE = 0.549f,
                BG_GREEN = 0.9059f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
              VIEWPORT_Y = 0,
              VIEWPORT_WIDTH  = WINDOW_WIDTH,
              VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
               F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;
constexpr char  SPRITESHEET_FILEPATH[] = "ship.png",
                FONTSHEET_FILEPATH[] = "assets/font1.png",
                PLATFORM_FILEPATH[]    = "assets/platform.png";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL    = 0;
constexpr GLint TEXTURE_BORDER     = 0;

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
constexpr int   PLATFORM_COUNT = 2;
constexpr float ROTATION_SPEED = 0.025f;
constexpr float THRUST_STRENGTH = 0.025f;

// ————— STRUCTS AND ENUMS —————//
enum AppStatus { RUNNING, TERMINATED };

struct GameState
{
    Entity* player;
    Entity* platforms;
};

// ————— VARIABLES ————— //
GameState g_game_state;

SDL_Window* g_display_window;
AppStatus g_app_status = RUNNING;

ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks   = 0.0f;
float g_time_accumulator = 0.0f;

GLuint g_font_texture_id;
constexpr int FONTBANK_SIZE = 16;

// ———— GENERAL FUNCTIONS ———— //
GLuint load_texture(const char* filepath);

void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, 
                                     STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, 
                 GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Lunar Lander",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
    if (context == nullptr)
    {
        LOG("ERROR: Could not create OpenGL context.\n");
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix       = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ————— PLAYER ————— //
    GLuint player_texture_id = load_texture(SPRITESHEET_FILEPATH);

    g_game_state.player = new Entity;

	g_game_state.player->set_texture_id(player_texture_id);
    g_game_state.player->set_scale(glm::vec3(0.5f, 0.75f, 0.0f));
    g_game_state.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.1, 0.0f));
    g_game_state.player->set_position(glm::vec3(-3.0f, 3, 0.0f));

    // ————— PLATFORM ————— //

    g_game_state.platforms = new Entity[PLATFORM_COUNT];

    g_game_state.platforms[0].set_texture_id(load_texture(PLATFORM_FILEPATH));
    g_game_state.platforms[0].set_position(glm::vec3(-2.0f, -2.5f, 0.0f));
    g_game_state.platforms[0].update(0.0f, nullptr, 0);

    g_game_state.platforms[1].set_texture_id(load_texture(PLATFORM_FILEPATH));
    g_game_state.platforms[1].set_position(glm::vec3(2.5f, -3.0f, 0.0f));
    g_game_state.platforms[1].update(0.0f, nullptr, 0);

    g_font_texture_id = load_texture(FONTSHEET_FILEPATH);

    // ————— GENERAL ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
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

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])       g_game_state.player->set_rotation(g_game_state.player->get_rotation() + ROTATION_SPEED);
    else if (key_state[SDL_SCANCODE_RIGHT]) g_game_state.player->set_rotation(g_game_state.player->get_rotation() - ROTATION_SPEED);

    if (key_state[SDL_SCANCODE_UP]) {
        if (g_game_state.player->get_fuel() > 0.0f) {
            float rad = glm::radians(g_game_state.player->get_rotation() + 90.0f);
            glm::vec3 thrust_vec = glm::vec3(cos(rad), sin(rad), 0.0f) * THRUST_STRENGTH;
            glm::vec3 new_acceleration = g_game_state.player->get_acceleration();
            new_acceleration += thrust_vec;
            g_game_state.player->set_acceleration(new_acceleration);
			
            float fuel_used = FIXED_TIMESTEP;
            float current_fuel = g_game_state.player->get_fuel();
            current_fuel -= fuel_used;
            g_game_state.player->set_fuel(current_fuel);
        }
    }
}

void draw_text(ShaderProgram* shader_program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for
    // each character. Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their
        //    position relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Insert the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    shader_program->set_model_matrix(model_matrix);
    glUseProgram(shader_program->get_program_id());

    glVertexAttribPointer(shader_program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices.data());
    glEnableVertexAttribArray(shader_program->get_position_attribute());

    glVertexAttribPointer(shader_program->get_tex_coordinate_attribute(), 2, GL_FLOAT,
        false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(shader_program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(shader_program->get_position_attribute());
    glDisableVertexAttribArray(shader_program->get_tex_coordinate_attribute());
}

void update()
{
    // ————— DELTA TIME ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    if (g_game_state.player->get_lose() || g_game_state.player->get_win()) return;

    // ————— FIXED TIMESTEP ————— //
    // STEP 1: Keep track of how much time has passed since last step
    delta_time += g_time_accumulator;

    // STEP 2: Accumulate the ammount of time passed while we're under our fixed timestep
    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    // STEP 3: Once we exceed our fixed timestep, apply that elapsed time into the 
    //         objects' update function invocation
    while (delta_time >= FIXED_TIMESTEP)
    {
        // Notice that we're using FIXED_TIMESTEP as our delta time
        g_game_state.player->update(FIXED_TIMESTEP, g_game_state.platforms, 
                                    PLATFORM_COUNT);

        g_game_state.platforms[0].move_platform(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }

    g_time_accumulator = delta_time;
}

void render()
{
    // ————— GENERAL ————— //
    glClear(GL_COLOR_BUFFER_BIT);

    // ————— PLAYER ————— //
    g_game_state.player->render(&g_shader_program);

    // ————— PLATFORM ————— //
    for (int i = 0; i < PLATFORM_COUNT; i++) 
        g_game_state.platforms[i].render(&g_shader_program);

    //TEXT
	std::string cur_fuel = "FUEL: " + std::to_string((int)g_game_state.player->get_fuel());
    draw_text(&g_shader_program, g_font_texture_id,
        cur_fuel, 0.35f, 0.01f,
        glm::vec3(-4.8f, 3.2f, 0.0f));

    if (g_game_state.player->get_win()) {
        draw_text(&g_shader_program, g_font_texture_id,
            "MISSION ACCOMPLISHED!", 0.5f, 0.01f,
            glm::vec3(-4.8f, 0.0f, 0.0f));
    }
    else if (g_game_state.player->get_lose()) {
        draw_text(&g_shader_program, g_font_texture_id,
            "MISSION FAILED!", 0.5f, 0.01f,
            glm::vec3(-3.5f, 0.0f, 0.0f));
    }

    // ————— GENERAL ————— //
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() 
{
    SDL_Quit();
    
    delete   g_game_state.player;
    delete[] g_game_state.platforms;
}


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
