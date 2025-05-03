#pragma once
#include "Scene.h"

class LevelD : public Scene {
public:
    int ENEMY_COUNT = 2;

    ~LevelD();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};