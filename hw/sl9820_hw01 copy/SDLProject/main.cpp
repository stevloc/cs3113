/**
* Author: Steven Lo
* Assignment: Simple 2D Scene
* Date due: 2024-02-17, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

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
#include <cstdlib>

enum Coordinate {
    x_coordinate,
    y_coordinate
};

#define LOG(argument) std::cout << argument << '\n'

const int WINDOW_WIDTH = 640,
          WINDOW_HEIGHT = 480;

float BG_RED = 0.1922f,
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

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;

const char PLAYER_SPRITE_FILEPATH[] = "assets/mario.png";
const char GOOMPA_FILEPATH[] = "assets/goomba.png";
const char TURTLE_FILEPATH[] = "assets/turtle.png";

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool g_is_growing = true;
float MINIMUM_COLLISION_DISTANCE = 1.0f;

ShaderProgram g_shader_program;
glm::mat4 view_matrix, g_projection_matrix, g_trans_matrix, g_model_marix, g_turtle_model_matrix, g_goompa_model_matrix;

float g_previous_ticks = 0.0f;

GLuint g_player_texture_id;
GLuint g_goompa_texture_id;
GLuint g_turtle_texture_id;

glm::vec3 g_player_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_goompa_position = glm::vec3(8.0f, 0.0f, 0.0f);
glm::vec3 g_turtle_position = glm::vec3(4.0f, 0.0f, 0.0f);

float g_turtle_angle = glm::radians(1.5f);
const float TURTLE_ROT_SPEED = 2;

const float GOOMBA_SCALE_INCREMENT = 0.001f;
const float GOOMBA_MAX_SCALE = 5.0f;
float g_goompa_scale_factor = 1.0f;

float deltaTimeAccumulator = 0.0f;

GLuint load_texture(const char* filepath) {

    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL){
        LOG("Unable to load image. Make sure the path is correct.");
        LOG(filepath);
        assert(false);
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

    g_display_window = SDL_CreateWindow("Hello, Game 1!",
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
    
    g_player_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
    g_goompa_texture_id = load_texture(GOOMPA_FILEPATH);
    g_turtle_texture_id = load_texture(TURTLE_FILEPATH);
    
    view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(view_matrix);
    
    float initialX = -4.5f;
    float initialY = -3.25f;
    
    g_player_position = glm::vec3(initialX, initialY, 0.0f);
    g_goompa_position = glm::vec3(initialX - 2.0f, initialY - 2.0f, 0.0f);
    g_turtle_position = glm::vec3(initialX - 4.0f, initialY - 4.0f, 0.0f);
    
    g_model_marix = glm::scale(g_model_marix, glm::vec3(9.0f, 9.0f, 1.0f));
    g_goompa_model_matrix = glm::scale(g_goompa_model_matrix, glm::vec3( 1.0f, 2.0f, 1.0f));
    g_turtle_model_matrix = glm::scale(g_turtle_model_matrix, glm::vec3( 1.0f, 2.0f, 1.0f));
    
    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            g_game_is_running = false;
        }}}

void update(){
    
    float ticks = (float) SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    float diagonalSpeedX = 0.5f;
    float diagonalSpeedY = 0.5f;
    
    g_player_position += glm::vec3(diagonalSpeedX, diagonalSpeedY, 0.0f) * delta_time;
    g_goompa_position += glm::vec3(diagonalSpeedX, diagonalSpeedY, 0.0f) * delta_time;
    g_turtle_position += glm::vec3(diagonalSpeedX, diagonalSpeedY, 0.0f) * delta_time;

    g_model_marix = glm::mat4(1.0f);
    g_model_marix = glm::translate(g_model_marix, g_player_position);
    g_model_marix = glm::scale(g_model_marix, glm::vec3(1.4f, 2.0f, 1.0f));

    g_turtle_angle += TURTLE_ROT_SPEED * delta_time;
    g_turtle_model_matrix = glm::mat4(1.0f);
    g_turtle_model_matrix = glm::translate(g_turtle_model_matrix, g_turtle_position);
    g_turtle_model_matrix = glm::rotate(g_turtle_model_matrix, g_turtle_angle, glm::vec3(0.0f, 0.0f, 1.0f));
    
    g_goompa_scale_factor += GOOMBA_SCALE_INCREMENT;
    
    if (g_goompa_scale_factor > GOOMBA_MAX_SCALE) {
        g_goompa_scale_factor = GOOMBA_MAX_SCALE;
    }
    
    g_goompa_model_matrix = glm::mat4(1.0f);
    g_goompa_model_matrix = glm::translate(g_goompa_model_matrix, g_goompa_position);
    g_goompa_model_matrix = glm::scale(g_goompa_model_matrix, glm::vec3(g_goompa_scale_factor, g_goompa_scale_factor, 1.0f));
    
    deltaTimeAccumulator += delta_time;
    
    if (deltaTimeAccumulator >= 2.0f) {
        deltaTimeAccumulator = 0.0f;
        BG_RED = rand() % 256 / 255.0f;
        BG_GREEN = rand() % 256 / 255.0f;
        BG_BLUE = rand() % 256 / 255.0f;
    }
}

void draw_object(glm::mat4 &object_model_matrix, GLuint &object_texture_id) {
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
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
    
    draw_object(g_model_marix, g_player_texture_id);
    draw_object(g_goompa_model_matrix, g_goompa_texture_id);
    draw_object(g_turtle_model_matrix, g_turtle_texture_id);
    
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


