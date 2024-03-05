/**
* Author: Steven Lo
* Assignment: Pong Clone
* Date due: 2024-03-02, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'

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

#include <cstdlib>

enum Coordinate {
    x_coordinate,
    y_coordinate
};

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

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const float DEGREES_PER_SECOND = 90.0f;
const float MINIMUM_COLLISION_DISTANCE = 1.0f;

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

const char PD1_SPRITE[] = "assets/pd1.png";
const char PD2_SPRITE[] = "assets/pd2.png";
const char COIN_SPRITE[] = "assets/coin.png";

glm::mat4 g_view_matrix, g_projection_matrix;
glm::mat4 g_model_matrix_pd1, g_model_matrix_pd2, g_model_matrix_coin;

const glm::vec3     PD1_POSITION = glm::vec3(-4.5f, 0.0f, 0.0f), PD2_POSITION = glm::vec3(4.5f, 0.0f, 0.0f),
                    COIN_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);

const glm::vec3     PD_SIZE = glm::vec3(0.5f, 2.5f, 1.0f),
                    COIN_SIZE = glm::vec3(0.5f, 0.5f, 0.0f);

ShaderProgram g_shader_program;
GLuint  g_texture_id_pd1, g_texture_id_pd2, g_texture_id_coin;

glm::vec3   movement_pd1 = glm::vec3(0.0f, 0.0f, 0.0f), movement_pd2 = glm::vec3(0.0f, 0.0f, 0.0f),
            movement_coin = glm::vec3(1.0f, 1.0f, 0.0f);

glm::vec3   position_pd1 = glm::vec3(-4.5f, 0.0f, 0.0f), position_pd2 = glm::vec3(4.5f, 0.0f, 0.0f), position_coin = glm::vec3(0.0f, 0.0f, 0.0f);

const float SPEED_PD = 20.0f, SPEED_COIN = 1.0f, ROT_SPEED_BALL = 45.0f, SPEED_PD_AUTO = 1.0f;
float rot_angle = 0.0f;
float g_previous_ticks = 0.0f;

SDL_Window* g_display_window;
bool g_game_is_running = true, game_ended = false, pd2_auto = false, pd2_auto_direction = true;

int player1_score = 0, player2_score = 0;
bool coin_direc_y = true, coin_direc_x = true;


bool out(const glm::vec3 &init_position, const glm::vec3 &displacement, const glm::vec3 &scale_vector) {
    float window_height = 3.75f;
    float boundary_limit = window_height - 0.5f * scale_vector.y;
    glm::vec3 curr_position = init_position + displacement;
    return curr_position.y <= -boundary_limit || curr_position.y >= boundary_limit;
}


bool ball_hits_vertical_wall (const glm::vec3 &init_position, glm::vec3 &position) {
    float window_width = 5.0f;
    float boundary_limit = window_width - 0.5f * COIN_SIZE.x;
    glm::vec3 curr_position = init_position + position;
    if (curr_position.x <= -boundary_limit or curr_position.x >= boundary_limit)
    { return true; }
    else { return false; }
}

bool check_collision(glm::vec3 &position_a, glm::vec3 &position_b)
{
    return sqrt(
                pow(position_b[0] - position_a[0], 2) +
                pow(position_b[1] - position_b[1], 2)
            ) < MINIMUM_COLLISION_DISTANCE;
}

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

void initialize_objects(GLuint& texture_id, const char* sprite, glm::mat4& model_matrix,
                   glm::mat4& view_matrix, glm::mat4& projection_matrix, const glm::vec3 init_position, glm::vec3 size_vector) {
    texture_id = load_texture(sprite);
    model_matrix = glm::mat4(1.0f);
    model_matrix  = glm::translate(model_matrix, init_position);
    model_matrix  = glm::scale(model_matrix, size_vector);

}

void initialise() {
    SDL_Init(SDL_INIT_VIDEO);

    g_display_window = SDL_CreateWindow("Mario Pong",
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
    
    g_model_matrix_pd1 = glm::mat4(1.0f);
    g_model_matrix_pd1 = glm::translate(g_model_matrix_pd1, PD1_POSITION);
    g_model_matrix_pd1 = glm::scale(g_model_matrix_pd1, PD_SIZE);
    
    g_model_matrix_pd2 = glm::mat4(1.0f);
    g_model_matrix_pd2 = glm::translate(g_model_matrix_pd2, PD2_POSITION);
    g_model_matrix_pd2 = glm::scale(g_model_matrix_pd2, PD_SIZE);
    
    g_model_matrix_coin = glm::mat4(1.0f);
    g_model_matrix_coin = glm::translate(g_model_matrix_coin, COIN_POSITION);
    g_model_matrix_coin = glm::scale(g_model_matrix_coin, COIN_SIZE);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glUseProgram(g_shader_program.get_program_id());
    
    
    g_texture_id_pd1 = load_texture(PD1_SPRITE);
    g_texture_id_pd2 = load_texture(PD2_SPRITE);
    g_texture_id_coin = load_texture(COIN_SPRITE);
    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
}

void process_input() {
    SDL_Event event;
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);
    
    while (SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                    g_game_is_running = !g_game_is_running;
                    break;
                
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_t:
                            pd2_auto = !pd2_auto;
                            break;
                        default: 
                            break;
                    }
            }
        }
        
    float paddle_speed = 0.1f;
        if (key_state[SDL_SCANCODE_W]) {
            movement_pd1.y += paddle_speed;
        }
        if (key_state[SDL_SCANCODE_S]) {
            movement_pd1.y -= paddle_speed;
        }

        if (!pd2_auto) {
            if (key_state[SDL_SCANCODE_UP]) {
                movement_pd2.y += paddle_speed;
            }
            if (key_state[SDL_SCANCODE_DOWN]) {
                movement_pd2.y -= paddle_speed;
            }
        }

}

void user_move(const glm::vec3 &init_position, glm::vec3 &position, const glm::vec3 scale_vector, glm::vec3 &movement,
                      const float speed, glm::mat4 &model_matrix, float delta_time) {
    position += movement * speed * delta_time;
    if (out(init_position, position, scale_vector)) {
        position -= movement * speed * delta_time;
    }
    movement = glm::vec3(0.0f, 0.0f, 0.0f);
}


void auto_move(const glm::vec3 &init_position, glm::vec3 &position, const glm::vec3 scale_vector, glm::vec3 &movement,
                      const float speed, glm::mat4 &model_matrix, float delta_time){
    
    if (pd2_auto_direction) position.y += SPEED_PD_AUTO * delta_time;
    else position.y -= SPEED_PD_AUTO * delta_time;
    
    if (out(init_position, position, scale_vector)) pd2_auto_direction = !pd2_auto_direction;
    movement = glm::vec3(0.0f, 0.0f, 0.0f);
    
}


void update(){
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    if (pd2_auto) auto_move(PD2_POSITION, position_pd2, PD_SIZE, movement_pd2, SPEED_PD, g_model_matrix_pd2, delta_time);
    else user_move(PD2_POSITION, position_pd2, PD_SIZE, movement_pd2, SPEED_PD, g_model_matrix_pd2, delta_time);
    
    user_move(PD1_POSITION, position_pd1, PD_SIZE, movement_pd1, SPEED_PD, g_model_matrix_pd1, delta_time);
    
    position_coin += movement_coin * SPEED_COIN * delta_time;
    
    if (ball_hits_vertical_wall(PD1_POSITION, position_coin)) {
        game_ended = true;
        if ((PD1_POSITION + position_coin).x < 0) player2_score += 1;
        else player1_score += 1;
    } else {
        if (check_collision(position_coin, position_pd1) || check_collision(position_coin, position_pd2)) {
            position_coin += movement_coin * SPEED_COIN * delta_time;
            movement_coin = glm::vec3(-movement_coin.x, movement_coin.y, 0.0f);
        }
        
        if (out(COIN_POSITION, position_coin, COIN_SIZE)) {
            movement_coin = glm::vec3(movement_coin.x, -movement_coin.y, 0.0f);
            position_coin += movement_coin * SPEED_COIN * delta_time;
        }
    }
    
    rot_angle += ROT_SPEED_BALL * delta_time;
    g_model_matrix_coin = glm::rotate(g_model_matrix_coin, glm::radians(rot_angle), glm::vec3(0.0f, 0.0f, 1.0f));
    
    g_model_matrix_pd1 = glm::mat4(1.0f);
    g_model_matrix_pd1 = glm::translate(g_model_matrix_pd1, position_pd1);
    g_model_matrix_pd1 = glm::scale(g_model_matrix_pd1, PD_SIZE);
    
    g_model_matrix_pd2 = glm::mat4(1.0f);
    g_model_matrix_pd2 = glm::translate(g_model_matrix_pd2, position_pd2);
    g_model_matrix_pd2 = glm::scale(g_model_matrix_pd2, PD_SIZE);
    
    g_model_matrix_coin = glm::mat4(1.0f);
    g_model_matrix_coin = glm::translate(g_model_matrix_coin, position_coin);
    g_model_matrix_coin = glm::scale(g_model_matrix_coin, COIN_SIZE);
    
}


void draw_object(glm::mat4 &model_matrix, GLuint &texture_id){
        g_shader_program.set_model_matrix(model_matrix);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
void render() {
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());

    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    draw_object(g_model_matrix_pd1, g_texture_id_pd1);
    draw_object(g_model_matrix_pd2, g_texture_id_pd2);
    draw_object(g_model_matrix_coin, g_texture_id_coin);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[]) {
    initialise();
    
    while (g_game_is_running) {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}

