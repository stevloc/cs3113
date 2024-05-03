/**
* Author: Steven Lo
* Assignment: Farm's Ninja
* Date due: 2024-05-03, 3:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "Scene.h"

class LevelB : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 2;
    int counter = 60;
    float accumulator;

    // ————— CONSTRUCTOR ————— //
    ~LevelB();
    
    // ————— METHODS ————— //
    void initialise(int lives, int comp_levels) override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};

