/**
* Author: Christopher Haddad
* Assignment: Pong Clone
* Date due: 2025-3-01, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>

#include <cstdlib>

enum AppStatus { RUNNING, TERMINATED };

constexpr float WINDOW_SIZE_MULT = 1.5f;

constexpr int WINDOW_WIDTH  = 640 * WINDOW_SIZE_MULT,
              WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED     = 0.9765625f,
                BG_GREEN   = 0.97265625f,
                BG_BLUE    = 0.9609375f,
                BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL    = 0;
constexpr GLint TEXTURE_BORDER     = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char OMNI_SPRITE_FILEPATH[] = "Omni-man.png",
               INVINCIBLE_SPRITE_FILEPATH[]  = "Invincible.png",
               BALL_SPRITE_FILEPATH[] = "Immortal.png",
               BALL2_SPRITE_FILEPATH[] = "Immortal.png",
               BALL3_SPRITE_FILEPATH[] = "Immortal.png",
               INVINCIBLE_WIN_FILEPATH[] = "Invincible_win.png",
               OMNI_WIN_FILEPATH[] = "Omni_win.png",
	           BACKGROUND_FILEPATH[] = "background.png";

constexpr glm::vec3 INIT_SCALE_INVINCIBLE = glm::vec3(0.75f, 2.0f, 0.0f),
                    INIT_POS_INVINCIBLE = glm::vec3(-4.0f, 0.0f, 0.0f),
                    INIT_SCALE_OMNI = glm::vec3(0.75f, 2.0f, 0.0f),
                    INIT_POS_OMNI = glm::vec3(4.0f, 0.0f, 0.0f),
                    INIT_SCALE_BALL = glm::vec3(1.0f, 1.0f, 1.0f),
                    INIT_POS_BALL = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_BALL2 = glm::vec3(1.0f, 1.0f, 1.0f),
                    INIT_POS_BALL2 = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_BALL3 = glm::vec3(1.0f, 1.0f, 1.0f),
                    INIT_POS_BALL3 = glm::vec3(0.0f, 0.0f, 0.0f),
                    INIT_SCALE_INVINCIBLE_WIN = glm::vec3(3.0f, 1.0f, 0.0f),
	                INIT_POS_INVINCIBLE_WIN = glm::vec3(0.0f, 0.0f, 0.0f),  
                    INIT_SCALE_OMNI_WIN = glm::vec3(3.0f, 1.0f, 0.0f),
                    INIT_POS_OMNI_WIN = glm::vec3(0.0f, 0.0f, 0.0f),
	                INIT_SCALE_BACKGROUND = glm::vec3(15.0f, 7.5f, 0.0f),
	                INIT_POS_BACKGROUND = glm::vec3(0.0f, 0.0f, 0.0f);

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_omni_matrix, g_projection_matrix, g_invincible_matrix, g_ball_matrix, g_ball2_matrix, g_ball3_matrix, g_invincible_win_matrix, g_omni_win_matrix, g_background_matrix;

float g_previous_ticks = 0.0f;

GLuint g_omni_texture_id;
GLuint g_invincible_texture_id;
GLuint g_ball_texture_id;
GLuint g_ball2_texture_id;
GLuint g_ball3_texture_id;
GLuint g_invincible_win_texture_id;
GLuint g_omni_win_texture_id;
GLuint g_background_texture_id;

constexpr float SPEED = 3.0f;

glm::vec3 g_omni_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_omni_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_invincible_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_invincible_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball_movement = glm::vec3(-1.0f, 0.0f, 0.0f);

glm::vec3 g_ball2_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball2_movement = glm::vec3(1.0f, 0.0f, 0.0f);

glm::vec3 g_ball3_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_ball3_movement = glm::vec3(-1.0f, 0.0f, 0.0f);

glm::vec3 g_invincible_win_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_omni_win_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_background_position = glm::vec3(0.0f, 0.0f, 0.0f);

bool single_player = true;
bool invincible_win = false;
bool omni_win = false;
int num_balls = 1;
bool moving_up = true;

void initialise();
void process_input();
void update();
void render();
void shutdown();

GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("PONG GAME",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);


    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_omni_matrix = glm::mat4(1.0f);
    g_invincible_matrix = glm::mat4(1.0f);
    g_ball_matrix = glm::mat4(1.0f);
	g_ball2_matrix = glm::mat4(1.0f);
	g_ball3_matrix = glm::mat4(1.0f);
    g_invincible_win_matrix = glm::mat4(1.0f);
    g_omni_win_matrix = glm::mat4(1.0f);
	g_background_matrix = glm::mat4(1.0f);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_omni_texture_id = load_texture(OMNI_SPRITE_FILEPATH);
    g_invincible_texture_id = load_texture(INVINCIBLE_SPRITE_FILEPATH);
	g_ball_texture_id = load_texture(BALL_SPRITE_FILEPATH);
	g_ball2_texture_id = load_texture(BALL2_SPRITE_FILEPATH);
	g_ball3_texture_id = load_texture(BALL3_SPRITE_FILEPATH);
	g_invincible_win_texture_id = load_texture(INVINCIBLE_WIN_FILEPATH);
	g_omni_win_texture_id = load_texture(OMNI_WIN_FILEPATH);
	g_background_texture_id = load_texture(BACKGROUND_FILEPATH);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_invincible_movement.y = 0.0f;
    g_omni_movement.y = 0.0f;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_q: g_app_status = TERMINATED; break;

                    case SDLK_t:
						single_player = !single_player;
						break;

                    case SDLK_1:
						num_balls = 1;
                        break;

					case SDLK_2:
						num_balls = 2;
						break;

					case SDLK_3:
						num_balls = 3;
						break;

                    default: break;
                }

            default:
                break;
        }
    }


    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    if ((key_state[SDL_SCANCODE_S]) && (g_invincible_position.y > -3.5f)) {
        g_invincible_movement.y = -1.0f;
    }
    else if ((key_state[SDL_SCANCODE_W]) && (g_invincible_position.y < 3.5f)) {
        g_invincible_movement.y = 1.0f;
	}

    if (single_player == false) {
        if ((key_state[SDL_SCANCODE_DOWN]) && (g_omni_position.y > -3.5f)) {
            g_omni_movement.y = -1.0f;
        }
        else if ((key_state[SDL_SCANCODE_UP]) && (g_omni_position.y < 3.5f)) {
            g_omni_movement.y = 1.0f;
        }
    }
}

void update()
{
    // --- DELTA TIME CALCULATIONS --- //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    // --- ACCUMULATOR LOGIC --- //
    g_invincible_position += g_invincible_movement * SPEED * delta_time;
    g_omni_position += g_omni_movement * SPEED * delta_time;

    if (num_balls == 1) {
        g_ball_position += g_ball_movement * SPEED * delta_time;
    }
    else if (num_balls == 2) {
        g_ball_position += g_ball_movement * SPEED * delta_time;
        g_ball2_position += g_ball2_movement * SPEED * delta_time;
    }
    else if (num_balls == 3) {
        g_ball_position += g_ball_movement * SPEED * delta_time;
        g_ball2_position += g_ball2_movement * SPEED * delta_time;
        g_ball3_position += g_ball3_movement * SPEED * delta_time;
    }

    if (single_player == true) {
        if (moving_up) {
            g_omni_movement.y = 1.0f;
            if (g_omni_position.y > 3.5) {
                moving_up = false;
            }
        }
        else {
            g_omni_movement.y = -1.0f;
            if (g_omni_position.y < -3.5) {
				moving_up = true;
            }
		}

        g_omni_position += g_omni_movement * SPEED * delta_time;
    }

    // --- TRANSLATION --- //
    g_omni_matrix = glm::mat4(1.0f);
    g_omni_matrix = glm::translate(g_omni_matrix, INIT_POS_OMNI);
    g_omni_matrix = glm::translate(g_omni_matrix, g_omni_position);

    g_invincible_matrix = glm::mat4(1.0f);
    g_invincible_matrix = glm::translate(g_invincible_matrix, INIT_POS_INVINCIBLE);
    g_invincible_matrix = glm::translate(g_invincible_matrix, g_invincible_position);

    g_ball_matrix = glm::mat4(1.0f);
    g_ball_matrix = glm::translate(g_ball_matrix, INIT_POS_BALL);
    g_ball_matrix = glm::translate(g_ball_matrix, g_ball_position);

    g_ball2_matrix = glm::mat4(1.0f);
    g_ball2_matrix = glm::translate(g_ball2_matrix, INIT_POS_BALL2);
    g_ball2_matrix = glm::translate(g_ball2_matrix, g_ball2_position);

    g_ball3_matrix = glm::mat4(1.0f);
    g_ball3_matrix = glm::translate(g_ball3_matrix, INIT_POS_BALL3);
    g_ball3_matrix = glm::translate(g_ball3_matrix, g_ball3_position);

    g_invincible_win_matrix = glm::mat4(1.0f);
    g_invincible_win_matrix = glm::translate(g_invincible_win_matrix, INIT_POS_INVINCIBLE_WIN);

    g_omni_win_matrix = glm::mat4(1.0f);
    g_omni_win_matrix = glm::translate(g_omni_win_matrix, INIT_POS_OMNI_WIN);

    g_background_matrix = glm::mat4(1.0f);
    g_background_matrix = glm::translate(g_background_matrix, INIT_POS_BACKGROUND);

    // --- SCALING --- //
    g_omni_matrix = glm::scale(g_omni_matrix, INIT_SCALE_OMNI);
    g_invincible_matrix = glm::scale(g_invincible_matrix, INIT_SCALE_INVINCIBLE);

    g_ball_matrix = glm::scale(g_ball_matrix, INIT_SCALE_BALL);
	g_ball2_matrix = glm::scale(g_ball2_matrix, INIT_SCALE_BALL2);
	g_ball3_matrix = glm::scale(g_ball3_matrix, INIT_SCALE_BALL3);

    g_invincible_win_matrix = glm::scale(g_invincible_win_matrix, INIT_SCALE_INVINCIBLE_WIN);
    g_omni_win_matrix = glm::scale(g_omni_win_matrix, INIT_SCALE_OMNI_WIN);
    g_background_matrix = glm::scale(g_background_matrix, INIT_SCALE_BACKGROUND);


    // --- COLLISION LOGIC --- //
    //ball1
    if (num_balls >= 1) {
        float invincible_x_distance = fabs(g_invincible_position.x + INIT_POS_INVINCIBLE.x - g_ball_position.x) -
            ((INIT_SCALE_BALL.x + INIT_SCALE_INVINCIBLE.x) / 3.0f);

        float invincible_y_distance = fabs(g_invincible_position.y + INIT_POS_INVINCIBLE.y - g_ball_position.y) -
            ((INIT_SCALE_BALL.y + INIT_SCALE_INVINCIBLE.y) / 3.0f);


        if (invincible_x_distance < 0.0f && invincible_y_distance < 0.0f)
        {
            g_ball_movement.x = -g_ball_movement.x;
            g_ball_movement.y = ((rand() % 201 - 100) / 100.0f);
        }

        float omni_x_distance = fabs(g_omni_position.x + INIT_POS_OMNI.x - g_ball_position.x) -
            ((INIT_SCALE_BALL.x + INIT_SCALE_INVINCIBLE.x) / 3.0f);

        float omni_y_distance = fabs(g_omni_position.y + INIT_POS_OMNI.y - g_ball_position.y) -
            ((INIT_SCALE_BALL.y + INIT_SCALE_OMNI.y) / 3.0f);


        if (omni_x_distance < 0.0f && omni_y_distance < 0.0f)
        {
            g_ball_movement.x = -g_ball_movement.x;
            g_ball_movement.y = ((rand() % 201 - 100) / 100.0f);
        }

        if (g_ball_position.y < -3.5f || g_ball_position.y > 3.5f)
        {
            g_ball_movement.y = -g_ball_movement.y;
        }

        if (g_ball_position.x < -4.5f) {
            g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            omni_win = true;
        }

        if (g_ball_position.x > 4.5f) {
            g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            invincible_win = true;
        }
    }

    //ball2
    if (num_balls >= 2) {
        float invincible_x_distance2 = fabs(g_invincible_position.x + INIT_POS_INVINCIBLE.x - g_ball2_position.x) -
            ((INIT_SCALE_BALL2.x + INIT_SCALE_INVINCIBLE.x) / 3.0f);

        float invincible_y_distance2 = fabs(g_invincible_position.y + INIT_POS_INVINCIBLE.y - g_ball2_position.y) -
            ((INIT_SCALE_BALL2.y + INIT_SCALE_INVINCIBLE.y) / 3.0f);


        if (invincible_x_distance2 < 0.0f && invincible_y_distance2 < 0.0f)
        {
            g_ball2_movement.x = -g_ball2_movement.x;
            g_ball2_movement.y = ((rand() % 201 - 100) / 100.0f);
        }

        float omni_x_distance2 = fabs(g_omni_position.x + INIT_POS_OMNI.x - g_ball2_position.x) -
            ((INIT_SCALE_BALL2.x + INIT_SCALE_INVINCIBLE.x) / 3.0f);

        float omni_y_distance2 = fabs(g_omni_position.y + INIT_POS_OMNI.y - g_ball2_position.y) -
            ((INIT_SCALE_BALL2.y + INIT_SCALE_OMNI.y) / 3.0f);


        if (omni_x_distance2 < 0.0f && omni_y_distance2 < 0.0f)
        {
            g_ball2_movement.x = -g_ball2_movement.x;
            g_ball2_movement.y = ((rand() % 201 - 100) / 100.0f);
        }

        if (g_ball2_position.y < -3.5f || g_ball2_position.y > 3.5f)
        {
            g_ball2_movement.y = -g_ball2_movement.y;
        }

        if (g_ball2_position.x < -4.5f) {
            g_ball2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            omni_win = true;
        }

        if (g_ball2_position.x > 4.5f) {
            g_ball2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            invincible_win = true;
        }
    }

    //ball3
    if (num_balls >= 3) {
        float invincible_x_distance3 = fabs(g_invincible_position.x + INIT_POS_INVINCIBLE.x - g_ball3_position.x) -
            ((INIT_SCALE_BALL3.x + INIT_SCALE_INVINCIBLE.x) / 3.0f);

        float invincible_y_distance3 = fabs(g_invincible_position.y + INIT_POS_INVINCIBLE.y - g_ball3_position.y) -
            ((INIT_SCALE_BALL3.y + INIT_SCALE_INVINCIBLE.y) / 3.0f);


        if (invincible_x_distance3 < 0.0f && invincible_y_distance3 < 0.0f)
        {
            g_ball3_movement.x = -g_ball3_movement.x;
            g_ball3_movement.y = ((rand() % 201 - 100) / 100.0f);
        }

        float omni_x_distance3 = fabs(g_omni_position.x + INIT_POS_OMNI.x - g_ball3_position.x) -
            ((INIT_SCALE_BALL3.x + INIT_SCALE_INVINCIBLE.x) / 3.0f);

        float omni_y_distance3 = fabs(g_omni_position.y + INIT_POS_OMNI.y - g_ball3_position.y) -
            ((INIT_SCALE_BALL3.y + INIT_SCALE_OMNI.y) / 3.0f);


        if (omni_x_distance3 < 0.0f && omni_y_distance3 < 0.0f)
        {
            g_ball3_movement.x = -g_ball3_movement.x;
            g_ball3_movement.y = ((rand() % 201 - 100) / 100.0f);
        }

        if (g_ball3_position.y < -3.5f || g_ball3_position.y > 3.5f)
        {
            g_ball3_movement.y = -g_ball3_movement.y;
        }

        if (g_ball3_position.x < -4.5f) {
            g_ball3_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            omni_win = true;
        }

        if (g_ball3_position.x > 4.5f) {
            g_ball3_movement = glm::vec3(0.0f, 0.0f, 0.0f);
            invincible_win = true;
        }
    }

    if (omni_win || invincible_win) {
        g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        g_ball2_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        g_ball3_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }

}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    // Bind texture
    draw_object(g_background_matrix, g_background_texture_id);
    draw_object(g_invincible_matrix, g_invincible_texture_id);
    draw_object(g_omni_matrix, g_omni_texture_id);

    if (num_balls >= 1) {
        g_ball_matrix = glm::mat4(1.0f);
        g_ball_matrix = glm::translate(g_ball_matrix, g_ball_position);
        g_ball_matrix = glm::scale(g_ball_matrix, INIT_SCALE_BALL);
        draw_object(g_ball_matrix, g_ball_texture_id);
    }

	if (num_balls >= 2) {
		g_ball2_matrix = glm::mat4(1.0f);
		g_ball2_matrix = glm::translate(g_ball2_matrix, g_ball2_position);
		g_ball2_matrix = glm::scale(g_ball2_matrix, INIT_SCALE_BALL2);
		draw_object(g_ball2_matrix, g_ball2_texture_id);
	}

	if (num_balls >= 3) {
		g_ball3_matrix = glm::mat4(1.0f);
		g_ball3_matrix = glm::translate(g_ball3_matrix, g_ball3_position);
		g_ball3_matrix = glm::scale(g_ball3_matrix, INIT_SCALE_BALL3);
		draw_object(g_ball3_matrix, g_ball3_texture_id);
	}

	if (invincible_win == true) {
		draw_object(g_invincible_win_matrix, g_invincible_win_texture_id);
	}
    else if (omni_win == true) {
        draw_object(g_omni_win_matrix, g_omni_win_texture_id);
    }

    // We disable two attribute arrays now
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


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

