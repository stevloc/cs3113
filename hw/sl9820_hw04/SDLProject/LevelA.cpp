/**
* Author: Steven Lo
* Assignment: Rise of the AI
* Date due: 2024-03-23, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 17
#define LEVEL_HEIGHT 8

unsigned int LEVEL_DATA[] =
{

    36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,
    36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,
    36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,
    36,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,
    36,  0,  0, 26, 27, 28,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 36,
    36,  0,  0,  0,  0,  0,  0, 17, 19,  0,  0,  0,  0,  0,  0,  0, 36,
    36,  0,  0,  0,  0,  0, 17, 34, 34, 19,  0,  0,  0, 17, 18, 19, 36,
    36, 17, 18, 18, 18, 18, 34, 34, 34, 34, 18, 18, 18, 34, 34, 35, 36
};

LevelA::~LevelA()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;

}

void LevelA::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA, map_texture_id, 1.0f, 16, 7);
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/player.png");
    
    // Walking
    m_state.player->m_walking[m_state.player->LEFT]  = new int[4] { 0, 3, 5, 9 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4] { 0, 3, 5, 9 };
    m_state.player->m_walking[m_state.player->UP]    = new int[4] { 0, 4, 6, 8 };
    m_state.player->m_walking[m_state.player->DOWN]  = new int[4] { 0, 4, 6, 8 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];
    m_state.player->m_animation_frames = 3;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 12;
    m_state.player->m_animation_rows   = 1;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);
    
    // Jumping
    m_state.player->m_jumping_power = 5.0f;
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("assets/enemies.png");
    

    
    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    
    m_state.enemies[0].set_position(glm::vec3(15.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_state.enemies[0].set_speed(1.0f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    m_state.enemies[0].m_walking[m_state.enemies[0].LEFT]  = new int[2] { 2, 3 };
    m_state.enemies[0].m_walking[m_state.enemies[0].RIGHT] = new int[2] { 2, 3 };
    
    m_state.enemies[0].m_animation_indices = m_state.enemies[0].m_walking[m_state.enemies[0].RIGHT];  // start George looking left
    m_state.enemies[0].m_animation_frames = 2;
    m_state.enemies[0].m_animation_index  = 0;
    m_state.enemies[0].m_animation_time   = 0.0f;
    m_state.enemies[0].m_animation_cols   = 9;
    m_state.enemies[0].m_animation_rows   = 3;
    m_state.enemies[0].set_height(0.8f);
    m_state.enemies[0].set_width(0.8f);
    
    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(WALKER_E1);
    m_state.enemies[1].m_texture_id = enemy_texture_id;
    
    m_state.enemies[1].set_position(glm::vec3(3.0f, 4.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.0f));
    m_state.enemies[1].set_speed(1.0f);
    m_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    m_state.enemies[1].m_walking[m_state.enemies[1].LEFT]  = new int[2] { 4, 5 };
    m_state.enemies[1].m_walking[m_state.enemies[1].RIGHT] = new int[2] { 4, 5 };
    
    m_state.enemies[1].m_animation_indices = m_state.enemies[1].m_walking[m_state.enemies[1].RIGHT];
    m_state.enemies[1].m_animation_frames = 2;
    m_state.enemies[1].m_animation_index  = 0;
    m_state.enemies[1].m_animation_time   = 0.0f;
    m_state.enemies[1].m_animation_cols   = 9;
    m_state.enemies[1].m_animation_rows   = 3;
    m_state.enemies[1].set_height(0.8f);
    m_state.enemies[1].set_width(0.8f);
    
    m_state.enemies[2].set_entity_type(ENEMY);
    m_state.enemies[2].set_ai_type(JUMPER);
    m_state.enemies[2].m_texture_id = enemy_texture_id;
    
    m_state.enemies[2].set_position(glm::vec3(9.0f, 2.0f, 0.0f));
    m_state.enemies[2].set_movement(glm::vec3(1.0f));
    m_state.enemies[2].set_speed(0.0f);
    m_state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    
    m_state.enemies[2].m_animation_indices = new int[1] { 6 };
    m_state.enemies[2].m_jumping_power = 3.0f;
    m_state.enemies[2].m_animation_frames = 1;
    m_state.enemies[2].m_animation_index  = 0;
    m_state.enemies[2].m_animation_time   = 0.0f;
    m_state.enemies[2].m_animation_cols   = 9;
    m_state.enemies[2].m_animation_rows   = 3;
    m_state.enemies[2].set_height(0.8f);
    m_state.enemies[2].set_width(0.8f);
 
}

void LevelA::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, NULL, NULL, m_state.map);
    }
    
    int temp_count = 0;
    for (size_t i = 0; i < ENEMY_COUNT ; i++){
        if (!m_state.enemies[i].isActive()) temp_count++;
    }
    
    if (temp_count == ENEMY_COUNT) game_won = true;
    if (!m_state.player->isActive()) game_over = true;
}

bool LevelA::game_done(){
    return game_over or game_won;
}


void LevelA::render(ShaderProgram *program)
{
    GLuint font_texture_id = Utility::load_texture("assets/font1.png");
    m_state.map->render(program);
    m_state.player->render(program);
    
    for (size_t i = 0; i < ENEMY_COUNT ; i++){
        if (m_state.enemies[i].isActive()){
            m_state.enemies[i].render(program);
        }
    }
    
    Utility::draw_text(program, font_texture_id, std::string("TO WIN YOU NEED TO"), 0.3f, 0.001f, glm::vec3(1.0f, -0.5f, 0.0f));
    Utility::draw_text(program, font_texture_id, std::string("KILL ALL THE ENEMIES"), 0.3f, 0.001f, glm::vec3(1.0f, -1.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, std::string("KILL: JUMP ON TOP"), 0.3f, 0.001f, glm::vec3(1.0f, -1.5f, 0.0f));
    
    if (game_over){
        Utility::draw_text(program, font_texture_id, std::string("You Lose"), 0.5f, 0.0f, m_state.player->get_position());
    }
    if (game_won){
        Utility::draw_text(program, font_texture_id, std::string("You Win"), 0.5f, 0.0f, m_state.player->get_position());
    }
    
}
