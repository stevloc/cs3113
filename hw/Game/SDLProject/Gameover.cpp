/**
* Author: Steven Lo
* Assignment: Platformer
* Date due: 2024-04-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Gameover.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

Gameover::~Gameover()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void Gameover::initialise(int lives) {
    m_state.next_scene_id = -1;
}

void Gameover::update(float delta_time){}

void Gameover::render(ShaderProgram *program){
    GLuint font_texture_id = Utility::load_texture("assets/font1.png");
    Utility::draw_text(program, font_texture_id, std::string("GAME OVER"), 0.5f, 0.0f, glm::vec3(-2.0f, 3.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, std::string("Press Q to Quit"), 0.5f, 0.0f, glm::vec3(-4.0f, 1.0f, 0.0f));
  
}


