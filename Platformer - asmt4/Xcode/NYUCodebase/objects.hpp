//
//  objects.hpp
//  NYUCodebase
//
//  Created by Brian Quinn on 11/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef objects_hpp
#define objects_hpp

#include <stdio.h>
#include "ShaderProgram.h"

using namespace std;

enum class EntityType {Player, Skeleton, Bat, Spider, Blob};

GLuint LoadTexture(const char* image_path);

struct Entity
{
    EntityType kind;
    Matrix modelMatrix;
    
    GLuint textureID;
    int index;
    int horizontalNum;
    int verticalNum;
    float spriteX;
    float spriteY;
    
    float u;
    float v;
    float height;
    float width;
    float scale;
    bool gravity;
    
    pair<float,float> position;
    pair<float,float> velocity;
    pair <float,float> acceleration;
    
    Entity(string& type, float xCoordinate, float yCoordinate, int idx, int xSprites, int ySprites, float scale = 1.0f);
    
    void setSpriteCoords(int index);
    void update(float elapsed);
    void render(ShaderProgram* program);
    bool collision(Entity* entity);
    void resolveCollision();
};

struct Player : Entity
{
//    Matrix playerView;
    static Player& getInstance(string& type, float xCoordinate, float yCoordinate);
    
    Player(string& type, float xCoordinate, float yCoordinate);
    Player& operator=(const Player&) = delete;
    Player(const Player&) = delete;
};

struct Enemy : Entity
{
    Enemy(string& type, float xCoordinate, float yCoordinate);
};

struct Object : Entity
{
    Object(string& type, float xCoordinate, float yCoordinate);
};

#endif /* objects_hpp */
