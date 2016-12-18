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
#include "gamestate.hpp"

extern float TILE_SIZEX;
extern float TILE_SIZEY;

using namespace std;

enum class EntityType {Player = 8, Skeleton = 9, Bat = 51, Spider = 57, Blob = 48};
struct Level;

GLuint LoadTexture(const char* image_path);
float lerp(float v0, float v1, float t);
void worldToTileCoordinates(float worldX, float worldY, int* gridX, int* gridY);

struct Entity
{
    EntityType kind;
    Matrix modelMatrix;
    Level* level;
    
    GLuint textureID;
    int index;
    int horizontalNum;
    int verticalNum;
    float spriteX;
    float spriteY;
    
    float animation;
    
    float u;
    float v;
    float height;
    float width;
    float scale;
    bool gravity;
    bool jump;
    bool friction;
    
    pair<float,float> position;
    pair<int,int> gridPosition;
    pair<float,float> velocity;
    pair <float,float> acceleration;
    
    Entity(string& type, float xCoordinate, float yCoordinate, int idx, int xSprites, int ySprites, float scale = 1.5f);
    
    void setSpriteCoords(int index);
    void update(float elapsed);
    void render(ShaderProgram* program);
    bool resolveCollision();
    bool collision(Entity* entity);
    
    virtual void attack() = 0;
};

struct Player : Entity
{
    Matrix playerView;
    static Player& getInstance(string& type, float xCoordinate, float yCoordinate);
    void attack();
    
protected:
    Player(string& type, float xCoordinate, float yCoordinate);
    Player& operator=(const Player&) = delete;
    Player(const Player&) = delete;
};

struct Enemy : Entity
{
    Enemy(string& type, float xCoordinate, float yCoordinate);
    void attack();
    float initHeight;
    float initSpeed;
};

struct Object : Entity
{
    Object(string& type, float xCoordinate, float yCoordinate);
    void attack();
};

#endif /* objects_hpp */
