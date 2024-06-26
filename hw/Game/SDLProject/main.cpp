/**
* Author: Steven Lo
* Assignment: Platformer
* Date due: 2024-04-13, 11:59pm
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
//#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "Menu.h"
#include "LevelC.h"
#include "Gameover.h"
#include "Effects.h"

// ––––– CONSTANTS ––––– //
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

const float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;


// ––––– GLOBAL VARIABLES ––––– //
Scene  *g_current_scene;
Menu *menu;
LevelA *g_levelA;
LevelB *g_levelB;
LevelC *g_levelC;
Gameover *gameover;
bool gamewin = false;
bool gamelost = false;
int g_n_lives = 3;
Effects *g_effects;
Scene   *g_levels[5];

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;
bool g_is_paused = false;

bool g_is_colliding_bottom = false;

// ––––– GENERAL FUNCTIONS ––––– //
void switch_to_scene(Scene *scene, int& lives){
    g_current_scene = scene;
    g_current_scene->initialise(lives); // DON'T FORGET THIS STEP!
}

void initialise() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Game!",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    menu = new Menu();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_levelC = new LevelC();
    gameover = new Gameover();
    
    g_levels[0] = menu;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_levelC;
    g_levels[4] = gameover;
    
    // Start at level A

    g_effects = new Effects(g_projection_matrix, g_view_matrix);
    switch_to_scene(g_levels[0], g_n_lives);
    

}

void process_input() {
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    if(g_current_scene != menu and g_current_scene != gameover){
        g_current_scene->m_state.player->set_movement(glm::vec3(0.0f));
    }
    
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
                        
                    case SDLK_SPACE:
                        // Jump
                        if (!(g_current_scene == menu or g_current_scene == gameover)){
                            if (g_current_scene->m_state.player->m_collided_bottom)
                            {
                                g_current_scene->m_state.player->m_is_jumping = true;
                                Mix_PlayChannel(-1, g_current_scene->m_state.jump_sfx, 0);
                            }
                        }
                        break;
                        
                    case SDLK_RETURN:
                        if (g_current_scene == menu){
                            switch_to_scene(g_levelA, g_n_lives);
                            g_effects = new Effects(g_projection_matrix, g_view_matrix);
                            g_effects->start(SHRINK, 6.0f);
                        }
                        break;
                        
                    case SDLK_p:
                        if (!(g_current_scene == menu or g_current_scene == gameover)){
                            g_is_paused = !g_is_paused;
                        }
                        break;
                        
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    if (!(g_current_scene == menu or g_current_scene == gameover)){
        const Uint8 *key_state = SDL_GetKeyboardState(NULL);
        
        if (key_state[SDL_SCANCODE_LEFT])
        {
            g_current_scene->m_state.player->move_left();
            g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->LEFT];
        }
        else if (key_state[SDL_SCANCODE_RIGHT])
        {
            g_current_scene->m_state.player->move_right();
            g_current_scene->m_state.player->m_animation_indices = g_current_scene->m_state.player->m_walking[g_current_scene->m_state.player->RIGHT];
        }
        
        // This makes sure that the player can't move faster diagonally
        if (glm::length(g_current_scene->m_state.player->get_movement()) > 1.0f)
        {
            g_current_scene->m_state.player->set_movement(glm::normalize(g_current_scene->m_state.player->get_movement()));
        }
    }
}

void update()
{
    
    if (g_is_paused) {
            return; 
    }
    
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        g_current_scene->update(FIXED_TIMESTEP);
        g_effects->update(FIXED_TIMESTEP);
        if (!(g_current_scene == menu or g_current_scene == gameover)) g_is_colliding_bottom = g_current_scene->m_state.player->m_collided_bottom;
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    // Prevent the camera from showing anything outside of the "edge" of the level
    g_view_matrix = glm::mat4(1.0f);
    
    if (!(g_current_scene == menu or g_current_scene == gameover)){
        
        if (g_current_scene->m_state.player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->m_state.player->get_position().x, 3.75, 0));
        } else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
        

        
        int g_next_scene_id = g_current_scene->m_state.next_scene_id;
        if (!gamewin && !gamelost) {
            g_n_lives = g_current_scene->n_lives;
            if (g_next_scene_id == 5) gamewin = true;
            else if (g_next_scene_id != -1) {
                if (g_next_scene_id != 4) {
                    g_effects->start(SHRINK, 6.0f);
                }
                switch_to_scene(g_levels[g_next_scene_id], g_n_lives);
            }
        }
    }


}

void render() {
    GLuint font_texture_id = Utility::load_texture("assets/font1.png");
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);
 
    glUseProgram(g_shader_program.get_program_id());
    g_current_scene->render(&g_shader_program);
    g_effects->render();
    if (gamewin){
        Utility::draw_text(&g_shader_program, font_texture_id, std::string("You Win"), 0.5f, 0.0f, glm::vec3(14.0f,-5.0f, 0.0f));
        Utility::draw_text(&g_shader_program, font_texture_id, std::string("Press Q to quit"), 0.2f, 0.0f, glm::vec3(14.0f,-5.5f, 0.0f));
    }
    
    
//    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    
    delete menu;
    delete g_levelA;
    delete g_levelB;
    delete g_levelC;
    delete gameover;
    delete g_effects;
}

// ––––– DRIVER GAME LOOP ––––– //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_game_is_running)
    {
        process_input();
        if (!gamewin) update();
        
        if (g_current_scene->m_state.next_scene_id >= 0 && g_current_scene->m_state.next_scene_id <= 4)
            switch_to_scene(g_levels[g_current_scene->m_state.next_scene_id], g_n_lives);
        
        render();
    }
    
    shutdown();
    return 0;
}

