/**
* Author: Steven Lo
* Assignment: Lunar Lander
* Date due: 2024-09-03, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define NUMBER_OF_ENEMIES 3
#define FIXED_TIMESTEP 0.0166666f
#define ACC_OF_GRAVITY -9.81f
#define PLATFORM_COUNT 3

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <ctime>
#include "cmath"
#include "Entity.h"

#include <cstdlib>

struct GameState
{
    Entity* lakitu;
    Entity* pipe;
    Entity* messages;
    Entity* fire;

};


// CONSTANTS

const int WINDOW_WIDTH = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
            BG_BLUE = 0.549f,
            BG_GREEN = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char  LAKITU_SPRITE[] = "assets/lakitu.png",
            PIPE_SPRITE[] = "assets/pipe.png",
            GAMEOVER_SPRITE[] = "assets/gameover.png",
            WIN_SPRITE[] = "assets/win.png",
            FIRE_SPRITE[] = "assets/fire.png";

const float MILLISECONDS_IN_SECOND = 1000.0;
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

// VARIABLES

GameState g_game_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_time_accumulator = 0.0f;
bool g_player_win = false;
bool g_player_lost = false;
bool game_ended = false;

float minX = -4.75f;
float maxX = 4.75f;
float minY = -2.75f;
float maxY = 3.25f;

int LAKITU_FUEL = 1000;


GLuint load_texture(const char* filepath) {
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL){
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert (false);
    }
    
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    
    return textureID;
}

void initialise() {
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("LAKITU LANDER",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    //PLAYER
    
    g_game_state.lakitu = new Entity();
    g_game_state.lakitu->m_texture_id = load_texture(LAKITU_SPRITE);
    g_game_state.lakitu->set_position(glm::vec3(-4.0f, 3.0f, 0.0f));
    
    g_game_state.lakitu->set_movement(glm::vec3(0.0f));
    g_game_state.lakitu->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.01, 0.0f));
    g_game_state.lakitu->set_speed(1.0f);
    g_game_state.lakitu->set_entity_type(PLAYER);
    g_game_state.lakitu->update(0.0f, NULL, 0.0f, g_player_win, g_player_lost);
    
    
    g_game_state.lakitu->m_walking[g_game_state.lakitu->LEFT]  = new int[4] { 4,   5,  6,  7 };
    g_game_state.lakitu->m_walking[g_game_state.lakitu->RIGHT] = new int[4] { 8,   9, 10, 12 };
    g_game_state.lakitu->m_walking[g_game_state.lakitu->UP]    = new int[4] { 12, 13, 14, 15 };
    g_game_state.lakitu->m_walking[g_game_state.lakitu->DOWN]  = new int[4] { 0,   1,  2,  3 };

    
    //PLATFORM
    g_game_state.pipe = new Entity();
    g_game_state.pipe->m_texture_id = load_texture(PIPE_SPRITE);
    g_game_state.pipe->set_position(glm::vec3(0.0f, -3.5f, 0.0f));
    g_game_state.pipe->update(0.0f, NULL, 0.0f, g_player_win, g_player_lost);
    g_game_state.pipe->set_entity_type(PLATFORM);

    
    // Obstacles
    g_game_state.fire = new Entity[5];
    g_game_state.fire[0].m_texture_id = load_texture(FIRE_SPRITE);
    g_game_state.fire[0].set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    g_game_state.fire[0].set_width(1.75f);
    g_game_state.fire[0].set_height(1.25f);
    g_game_state.fire[0].update(0.0f, NULL, 0.0f, g_player_win, g_player_lost);
    g_game_state.fire[0].m_model_matrix = glm::scale(g_game_state.fire[0].m_model_matrix, glm::vec3(0.50f, 0.50f, 0.0f));
    g_game_state.fire[0].set_entity_type(FIRE);
    
    g_game_state.fire[1].m_texture_id = load_texture(FIRE_SPRITE);
    g_game_state.fire[1].set_position(glm::vec3(3.0f, 2.0f, 0.0f));
    g_game_state.fire[1].set_width(1.75f);
    g_game_state.fire[1].set_height(1.25f);
    g_game_state.fire[1].update(0.0f, NULL, 0.0f, g_player_win, g_player_lost);
    g_game_state.fire[1].m_model_matrix = glm::scale(g_game_state.fire[1].m_model_matrix, glm::vec3(0.50f, 0.50f, 0.0f));
    g_game_state.fire[1].set_entity_type(FIRE);
    
    g_game_state.fire[2].m_texture_id = load_texture(FIRE_SPRITE);
    g_game_state.fire[2].set_position(glm::vec3(-3.0f, 2.0f, 0.0f));
    g_game_state.fire[2].set_width(1.75f);
    g_game_state.fire[2].set_height(1.25f);
    g_game_state.fire[2].update(0.0f, NULL, 0.0f, g_player_win, g_player_lost);
    g_game_state.fire[2].m_model_matrix = glm::scale(g_game_state.fire[2].m_model_matrix, glm::vec3(0.50f, 0.50f, 0.0f));
    g_game_state.fire[2].set_entity_type(FIRE);
 
    g_game_state.fire[3].m_texture_id = load_texture(FIRE_SPRITE);
    g_game_state.fire[3].set_position(glm::vec3(3.0f, -2.0f, 0.0f));
    g_game_state.fire[3].set_width(1.75f);
    g_game_state.fire[3].set_height(1.25f);
    g_game_state.fire[3].update(0.0f, NULL, 0.0f, g_player_win, g_player_lost);
    g_game_state.fire[3].m_model_matrix = glm::scale(g_game_state.fire[3].m_model_matrix, glm::vec3(0.50f, 0.50f, 0.0f));
    g_game_state.fire[3].set_entity_type(FIRE);
    
    g_game_state.fire[4].m_texture_id = load_texture(FIRE_SPRITE);
    g_game_state.fire[4].set_width(1.75f);
    g_game_state.fire[4].set_height(1.25f);
    g_game_state.fire[4].set_position(glm::vec3(-3.0f, -2.0f, 0.0f));
    g_game_state.fire[4].update(0.0f, NULL, 0.0f, g_player_win, g_player_lost);
    g_game_state.fire[4].m_model_matrix = glm::scale(g_game_state.fire[4].m_model_matrix, glm::vec3(0.50f, 0.50f, 0.0f));
    g_game_state.fire[4].set_entity_type(FIRE);
    
    // Message
    g_game_state.messages = new Entity[2];
    g_game_state.messages[0].m_texture_id = load_texture(WIN_SPRITE);
    g_game_state.messages[1].m_texture_id = load_texture(GAMEOVER_SPRITE);
    
    for (int i = 0; i < 2; i++) {
        g_game_state.messages[i].set_position(glm::vec3(0.0f));
        g_game_state.messages[i].m_model_matrix = glm::scale(g_game_state.messages[i].m_model_matrix, glm::vec3(5.0f, 3.0f, 1.0f));
        g_game_state.messages[i].set_entity_type(MESSAGE);
        g_game_state.messages[i].deactivate();
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

void process_input() {

    g_game_state.lakitu->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.001f, 0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
                // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        g_game_is_running = false;
                        break;
                        
                    default:
                        break;
                }
        }
    }
    
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
        {
            g_game_state.lakitu->move_left();
        }
        else if (key_state[SDL_SCANCODE_RIGHT])
        {
            g_game_state.lakitu->move_right();
        }
    
}


void update(){
        // DELTA TIME
        float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
        float delta_time = ticks - g_previous_ticks;
        g_previous_ticks = ticks;
        
        delta_time += g_time_accumulator;
        
        if (delta_time < FIXED_TIMESTEP) {
            g_time_accumulator = delta_time;
            return;
        }

        while (delta_time >= FIXED_TIMESTEP) {
            g_game_state.lakitu->update(FIXED_TIMESTEP, g_game_state.pipe, PLATFORM_COUNT, g_player_win, g_player_lost);
//            g_game_state.lakitu->update(FIXED_TIMESTEP, g_game_state.fire, PLATFORM_COUNT, g_player_win, g_player_lost);
            delta_time -= FIXED_TIMESTEP;
            
        }
    g_time_accumulator = delta_time;
    
    
    
    if (g_player_win) {
        game_ended = true;
        g_game_state.messages[0].activate();
    }
    else if (g_player_lost) {
        game_ended = true;
        g_game_state.messages[1].activate();
    }
}

    
void render() {
    
    // ————— GENERAL ————— //
    glClear(GL_COLOR_BUFFER_BIT);

    g_game_state.lakitu->render(&g_shader_program);
    g_game_state.pipe->render(&g_shader_program);

    for (int i = 0; i < 5; i++) g_game_state.fire[i].render(&g_shader_program);
    // ————— PLATFORM ————— //
    
    if (g_player_win) {
        g_game_state.messages[0].render(&g_shader_program);
       
    } else if (g_player_lost){
        g_game_state.messages[1].render(&g_shader_program);
    }
    
    // ————— GENERAL ————— //
    SDL_GL_SwapWindow(g_display_window);
    
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        if (game_ended == 0) {
            update();
        }
        render();
    }

    shutdown();
    return 0;
}


