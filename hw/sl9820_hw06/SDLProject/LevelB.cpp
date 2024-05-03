/**
* Author: Steven Lo
* Assignment: Farm's Ninja
* Date due: 2024-05-03, 3:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelB.h"
#include "Utility.h"

#define LEVEL_WIDTH 17
#define LEVEL_HEIGHT 8

unsigned int LevelB_FENCES[] ={
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,
     2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  0,  0,  0,  5,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,
     4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5
};

unsigned int LevelB_GRASS[] = {
    57, 60, 57, 57, 59, 60, 57, 57, 57, 57, 57, 59, 56, 57, 57, 57, 57,
    57, 57, 57, 57, 56, 60, 57, 57, 58, 57, 60, 59, 57, 60, 58, 56, 57,
    59, 58, 57, 57, 57, 57, 57, 58, 59, 57, 59, 59, 57, 57, 57, 57, 56,
    60, 57, 57, 57, 57, 59, 59, 59, 60, 60, 56, 57, 56, 57, 57, 57, 57,
    57, 57, 57, 57, 58, 59, 57, 58, 57, 57, 57, 56, 57, 57, 59, 57, 57,
    59, 57, 59, 57, 59, 57, 57, 57, 60, 58, 58, 58, 57, 57, 57, 56, 57,
    57, 57, 57, 57, 58, 59, 57, 56, 59, 57, 59, 58, 56, 59, 57, 57, 60,
    57, 60, 57, 57, 60, 58, 57, 57, 57, 57, 58, 58, 57, 57, 57, 57, 57
};

LevelB::~LevelB()
{
    delete [] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    delete    m_state.door;
    delete    m_state.water;
    delete    m_state.background;
    delete    m_state.prize;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeChunk(m_state.enemy);
    Mix_FreeMusic(m_state.bgm);
}

void LevelB::initialise(int lives, int comp_levels) {
    m_state.next_scene_id = -1;
    n_lives = lives;
    n_comp_levels = comp_levels;
    GLuint grass_texture_id = Utility::load_texture("assets/Grass.png");
    m_state.background = new Background(LEVEL_WIDTH, LEVEL_HEIGHT, LevelB_GRASS, grass_texture_id, 1.0f, 11, 7);
    GLuint fences_texture_id = Utility::load_texture("assets/Fences.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LevelB_FENCES, fences_texture_id, 1.0f, 5, 5);
    
    
    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
    // Existing
    m_state.prize = new Entity();
    m_state.prize->set_entity_type(EGG);
    m_state.prize->set_position(glm::vec3(1.0f, -7.0f, 0.0f));
    m_state.prize->m_texture_id = Utility::load_texture("assets/egg.png");
    m_state.prize->set_height(1.0f);
    m_state.prize->set_width(1.0f);
    
    m_state.door = new Entity();
    m_state.door->set_entity_type(OBJECT);
    m_state.door->set_position(glm::vec3(3.0f, 2.0f, 0.0f));
    m_state.door->m_texture_id = Utility::load_texture("assets/door.png");
    m_state.door->set_height(1.0f);
    m_state.door->set_width(0.7f);
    
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.0f);
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
    
    /**
     Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture("assets/chicken.png");
    
    m_state.enemies = new Entity[ENEMY_COUNT];
    
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(GUARD);
    m_state.enemies[0].m_texture_id = enemy_texture_id;
    m_state.enemies[0].set_position(glm::vec3(1.0f, -4.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
    m_state.enemies[0].set_speed(1.75f);
    m_state.enemies[0].m_animation_indices = new int[4] { 4,5,6,7 };
    m_state.enemies[0].m_animation_frames = 4;
    m_state.enemies[0].m_animation_index  = 0;
    m_state.enemies[0].m_animation_time   = 0.0f;
    m_state.enemies[0].m_animation_cols   = 4;
    m_state.enemies[0].m_animation_rows   = 2;
    m_state.enemies[0].set_height(0.8f);
    m_state.enemies[0].set_width(0.8f);
//    m_state.enemies[0].deactivate();
    
    
    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(GUARD);
    m_state.enemies[1].m_texture_id = enemy_texture_id;
    m_state.enemies[1].set_position(glm::vec3(2, -4.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
    m_state.enemies[1].set_speed(1.85f);
    m_state.enemies[1].m_animation_indices = new int[4] { 4,5,6,7 };
    m_state.enemies[1].m_animation_frames = 4;
    m_state.enemies[1].m_animation_index  = 0;
    m_state.enemies[1].m_animation_time   = 0.0f;
    m_state.enemies[1].m_animation_cols   = 4;
    m_state.enemies[1].m_animation_rows   = 2;
    m_state.enemies[1].set_height(0.8f);
    m_state.enemies[1].set_width(0.8f);
//    m_state.enemies[1].deactivate();

    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_state.bgm = Mix_LoadMUS("assets/background.ogg");
    m_state.jump_sfx = Mix_LoadWAV("assets/award.wav");
    m_state.enemy = Mix_LoadWAV("assets/enemy.wav");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    

 
}

void LevelB::update(float delta_time) {
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);
    m_state.player->update(delta_time, m_state.player, m_state.prize, 1, m_state.map);
    m_state.door->update(delta_time, m_state.player, NULL, NULL, m_state.map);
    accumulator += delta_time;
    if (accumulator > 1){
        counter -= 1;
        accumulator = 0;
    }
    m_state.prize->update(delta_time, m_state.player, NULL, NULL, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++) {
        m_state.enemies[i].update(delta_time, m_state.player, NULL, NULL, m_state.map);
        if (m_state.player->check_collision(&m_state.enemies[i])){
            Mix_PlayChannel(-1, m_state.enemy, 0);
            m_state.player->deactivate();
        }
    }
    
    if ((m_state.prize->isActive())){
        for (int i = 0; i < ENEMY_COUNT; i++) {
            m_state.enemies[i].deactivate();
        }
    } else if (!(m_state.prize->isActive())){
        for (int i = 0; i < ENEMY_COUNT; i++) {
            m_state.enemies[i].activate();
        }
    }
    
    
    if (!(m_state.player->isActive()) or counter < 0){
        n_lives -= 1;
        if (n_lives <= 0) {
            m_state.next_scene_id = 5;
        } else {
            m_state.player->activate();
            m_state.next_scene_id = 1;
            counter = 60;
        }
        return;
    }
    
    if (m_state.player->check_collision(m_state.prize)){
        for (int i = 0; i < ENEMY_COUNT; i++) {
            m_state.enemies[i].activate();
        }
    }
    
    if (m_state.player->check_collision(m_state.door)){
        counter = 60;
        if (!(m_state.prize->isActive())){
            n_comp_levels += 1;
        }
        m_state.next_scene_id = 1;
    }
    
    if (m_state.player->check_collision(m_state.prize)){
        Mix_PlayChannel(-1, m_state.jump_sfx, 0);
        m_state.prize->deactivate();
    }

    
}

void LevelB::render(ShaderProgram *program) {
    
    GLuint font_texture_id = Utility::load_texture("assets/font1.png");
    m_state.door->render(program);
    m_state.background->render(program);
    m_state.map->render(program);
    
    if (m_state.prize->isActive()){
        m_state.prize->render(program);
    }
    
    m_state.door->render(program);
    m_state.player->render(program);
    
    for (size_t i = 0; i < ENEMY_COUNT ; i++){
        m_state.enemies[i].render(program);
    }
    
    Utility::draw_text(program, font_texture_id, std::to_string(counter), 0.3f, 0.0f, m_state.player->get_position() + glm::vec3(-0.2f, 0.5f, 0.0f));


}



