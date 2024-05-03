/**
* Author: Steven Lo
* Assignment: Farm's Ninja
* Date due: 2024-05-03, 3:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Home.hpp"
#include "Utility.h"

#define LEVEL_WIDTH 11
#define LEVEL_HEIGHT 8

unsigned int HOME_FENCES[] ={
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4,
};

unsigned int HOME_GRASS[] = {
    57, 60, 57, 57, 57, 57, 57, 57, 57, 56, 56,
    57, 57, 57, 57, 56, 59, 58, 57, 57, 57, 56,
    59, 58, 57, 57, 57, 59, 57, 59, 57, 56, 56,
    60, 57, 57, 57, 60, 57, 60, 57, 57, 57, 56,
    57, 57, 57, 57, 58, 57, 57, 57, 57, 57, 56,
    59, 57, 59, 57, 56, 60, 57, 57, 57, 60, 56,
    57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 56,
    57, 60, 57, 57, 59, 57, 60, 57, 57, 57, 56,
};


Home::~Home()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.house;
    delete    m_state.door;
    delete    m_state.prize;
    delete    m_state.map;
    delete    m_state.background;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void Home::initialise(int lives, int comp_levels) {
    
    m_state.next_scene_id = -1;
    n_lives = lives;
    n_comp_levels = comp_levels;
    GLuint grass_texture_id = Utility::load_texture("assets/Grass.png");
    m_state.background = new Background(LEVEL_WIDTH, LEVEL_HEIGHT, HOME_GRASS, grass_texture_id, 1.0f, 11, 7);
    GLuint fences_texture_id = Utility::load_texture("assets/Fences.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, HOME_FENCES, fences_texture_id, 1.0f, 5, 5);
    
    m_state.house = new Entity();
    m_state.house->set_entity_type(OBJECT);
    m_state.house->set_position(glm::vec3(3.0f, 2.0f, 0.0f));
    m_state.house->m_texture_id = Utility::load_texture("assets/house.png");
    m_state.house->set_height(3.0f);
    m_state.house->set_width(3.0f);
    
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/player.png");
    
    // Walking
    m_state.player->m_walking[m_state.player->LEFT]  = new int[6] { 2, 6, 10, 14, 18, 22 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[6] { 3, 7, 11, 15, 19, 23 };
    m_state.player->m_walking[m_state.player->UP]    = new int[6] { 1, 5, 9, 13, 17, 21 };
    m_state.player->m_walking[m_state.player->DOWN]  = new int[6] { 0, 4, 8, 12 , 16, 20};

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->DOWN];
    m_state.player->m_animation_frames = 5;
    m_state.player->m_animation_index  = 0;
    m_state.player->m_animation_time   = 0.0f;
    m_state.player->m_animation_cols   = 4;
    m_state.player->m_animation_rows   = 7;
    m_state.player->set_height(0.7f);
    m_state.player->set_width(0.7f);
    
    m_state.door = new Entity[3];
    m_state.door[0].set_entity_type(OBJECT);
    m_state.door[0].set_position(glm::vec3(8.0f, -1.0f, 0.0f));
    m_state.door[0].m_texture_id = Utility::load_texture("assets/door.png");
    m_state.door[0].set_height(1.0f);
    m_state.door[0].set_width(0.7f);
    
    m_state.door[1].set_entity_type(OBJECT);
    m_state.door[1].set_position(glm::vec3(8.0f, -3.0f, 0.0f));
    m_state.door[1].m_texture_id = Utility::load_texture("assets/door.png");
    m_state.door[1].set_height(1.0f);
    m_state.door[1].set_width(0.7f);
    m_state.door[1].deactivate();
    
    m_state.door[2].set_entity_type(OBJECT);
    m_state.door[2].set_position(glm::vec3(8.0f, -5.0f, 0.0f));
    m_state.door[2].m_texture_id = Utility::load_texture("assets/door.png");
    m_state.door[2].set_height(1.0f);
    m_state.door[2].set_width(0.7f);
    m_state.door[2].deactivate();

    
    m_state.prize = new Entity();
    m_state.prize->set_entity_type(OBJECT);
    m_state.prize->set_position(glm::vec3(3.0f, -3.0f, 0.0f));
    m_state.prize->m_texture_id = Utility::load_texture("assets/chest.png");
    m_state.prize->set_height(0.7f);
    m_state.prize->set_width(1.0f);
    m_state.prize->deactivate();
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_state.bgm = Mix_LoadMUS("assets/home.ogg");
    m_state.jump_sfx = Mix_LoadWAV("assets/award.wav");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    

 
}

void Home::update(float delta_time) {
    m_state.player->update(delta_time, m_state.player, m_state.door, 3, m_state.map);
    m_state.player->update(delta_time, m_state.player, m_state.house, 1, m_state.map);
    m_state.player->update(delta_time, m_state.player, m_state.prize, 1, m_state.map);
    m_state.house->update(delta_time, m_state.player, NULL, NULL, m_state.map);
    m_state.prize->update(delta_time, m_state.player, NULL, NULL, m_state.map);
    m_state.door[0].update(delta_time, m_state.player, NULL, NULL, m_state.map);
    m_state.door[1].update(delta_time, m_state.player, NULL, NULL, m_state.map);
    m_state.door[2].update(delta_time, m_state.player, NULL, NULL, m_state.map);
//    std::cout << m_finished_level[0];
    
    if (m_state.player->check_collision(&m_state.door[0])){
        m_state.next_scene_id = 2;
    } else if (n_comp_levels >= 1 and m_state.player->check_collision(&m_state.door[1])){
        m_state.next_scene_id = 3;
    } else if (n_comp_levels >= 2 and m_state.player->check_collision(&m_state.door[2])) {
        m_state.next_scene_id = 4;
    }
    
    if (m_state.player->check_collision(m_state.prize)){
        Mix_PlayChannel(-1, m_state.jump_sfx, 0);
        m_state.next_scene_id = 6;
    }



//    if (!(m_state.player->isActive())){
//        n_lives -= 1;
//        if (n_lives <= 0) {
//            m_state.next_scene_id = 4;
//        } else {
//            m_state.player->activate();
//            m_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
//        }
//        return;
//    }
    // collision with door 1, go to level 1


    
}

void Home::render(ShaderProgram *program) {
    GLuint font_texture_id = Utility::load_texture("assets/font1.png");
    m_state.background->render(program);
    m_state.map->render(program);
    m_state.player->render(program);
    m_state.house->render(program);
    
    
//    if (m_state.prize->isActive()){
//        m_finished_level[0] = true;
//    }
    
    m_state.door[0].render(program);
    Utility::draw_text(program, font_texture_id, std::string("1"), 0.4f, 0.0f, glm::vec3(8.0f, -1.0f, 0.0f));
    
    
    Utility::draw_text(program, font_texture_id, std::string("PRESS P TO PAUSE"), 0.4f, 0.0f, glm::vec3(0.5f, -6.0f, 0.0f));
    Utility::draw_text(program, font_texture_id, std::string("PRESS Q TO QUIT"), 0.4f, 0.0f, glm::vec3(0.5f, -6.5f, 0.0f));

//    std::cout <<n_comp_levels;
    
    
    if (n_comp_levels >= 1){
        m_state.door[1].activate();
        m_state.door[1].render(program);
        Utility::draw_text(program, font_texture_id, std::string("2"), 0.4f, 0.0f, glm::vec3(8.0f, -3.0f, 0.0f));
    } 
    if (n_comp_levels >= 2 ){
        m_state.door[2].activate();
        m_state.door[2].render(program);
        Utility::draw_text(program, font_texture_id, std::string("3"), 0.4f, 0.0f, glm::vec3(8.0f, -5.0f, 0.0f));
    } 
    if (n_comp_levels >= 3){
        m_state.prize->activate();
        m_state.prize->render(program);
    }
    
    
    //    Utility::draw_text(program, font_texture_id, std::string("TO PAUSE"), 0.2f, 0.0f, glm::vec3(-1.5f, 2.0f, 0.0f));

}




