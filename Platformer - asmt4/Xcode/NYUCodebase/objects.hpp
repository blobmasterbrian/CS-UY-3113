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

enum class EntityType {Player, Enemy, Object, Platform, Intangible};

struct Entity
{
    EntityType kind;
    GLuint textureID;
    Matrix modelMatrix;
    float u;
    float v;
    float height;
    float width;
    pair<float,float> position;
    pair<float,float> velocity;
    pair <float,float> acceleration;
    
    Entity(string& type, float xCoordinate, float yCoordinate);
    
    void update(float elapsed);
    void render(ShaderProgram* program);
    bool collision(Entity* entity);
    void resolveCollision();
};

struct Player : Entity
{
    Matrix playerView;
    int index;
    int horizontalNum;
    int verticalNum;
    
    Player(string& type, float xCoordinate, float yCoordinate);
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
