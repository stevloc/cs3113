/**
* Author: Steven Lo
* Assignment: Farm's Ninja
* Date due: 2024-05-03, 3:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"
#include "Background.hpp"
#include <vector>

/**
    Notice that the game's state is now part of the Scene class, not the main file.
*/
struct GameState {
    Background *background;
    Map    *map;
    Map    *water;
    Entity *player;
    Entity *enemies;
    Entity *prize;
    Entity *house;
    Entity *door;
    
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    Mix_Chunk *effect;
    Mix_Chunk *enemy;
    Mix_Chunk *gameover;
    
    int next_scene_id;
};

class Scene {
public:
    // ————— ATTRIBUTES ————— //
    int m_number_of_enemies = 1;
    int n_lives = 3;
    int n_comp_levels;
    
    GameState m_state;
    
    // ————— METHODS ————— //
    virtual void initialise(int lives, int comp_levels) = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    // ————— GETTERS ————— //
    GameState const get_state()             const { return m_state;             }
    int       const get_number_of_enemies() const { return m_number_of_enemies; }
};

