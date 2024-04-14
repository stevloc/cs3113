#include "Scene.h"

class LevelC : public Scene {
public:
    int ENEMY_COUNT = 2;
    bool gameover = false;
    bool gamewin = false;
    
    ~LevelC();
    
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
    bool game_done();
};


