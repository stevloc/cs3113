/**
* Author: Steven Lo
* Assignment: Rise of the AI
* Date due: 2024-03-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

LevelX::~LevelX()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelX::initialise(int lives)
{
    m_state.next_scene_id = -1;
}

void LevelX::update(float delta_time)
{
}

void LevelA::render(ShaderProgram *program){
    Utility::draw_text(program, font_texture_id, std::string("PRESS ENTER TO START"), 0.5f, 0.0f, glm::vec3(1.0f, -1.0f, 0.0f));

}




