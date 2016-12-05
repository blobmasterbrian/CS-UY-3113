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

extern float TILE_SIZEX;
extern float TILE_SIZEY;

using namespace std;

enum class EntityType {Player, Skeleton, Bat, Spider, Blob};

GLuint LoadTexture(const char* image_path);
float lerp(float v0, float v1, float t);
void worldToTileCoordinates(float worldX, float worldY, int* gridX, int* gridY);

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
    pair<int,int> gridPosition;
    pair<float,float> velocity;
    pair <float,float> acceleration;
    
    Entity(string& type, float xCoordinate, float yCoordinate, int idx, int xSprites, int ySprites, float scale = 1.5f);
    
    void setSpriteCoords(int index);
    void update(float elapsed);
    void render(ShaderProgram* program);
    void resolveCollision();
    bool collision(Entity* entity);
};

struct Player : Entity
{
    Matrix playerView;
    static Player& getInstance(string& type, float xCoordinate, float yCoordinate);
    
protected:
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
