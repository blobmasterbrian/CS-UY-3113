//
//  objects.cpp
//  NYUCodebase
//
//  Created by Brian Quinn on 11/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "objects.hpp"

Entity::Entity(string& type, float xCoordinate, float yCoordinate, int idx, int xSprites, int ySprites, float scale): index(idx), horizontalNum(xSprites), verticalNum(ySprites), scale(scale), position(xCoordinate, yCoordinate)
{
    u = (float)(((int)index) % horizontalNum) / (float) horizontalNum;
    v = (float)(((int)index) / horizontalNum) / (float) verticalNum;
    spriteX = 1.0/(float)horizontalNum;
    spriteY = 1.0/(float)verticalNum;
}

void Entity::render(ShaderProgram* program)
{
    program->setModelMatrix(modelMatrix);
    float aspect = width/height;
    float vertices[] = {
        -scale * aspect, -scale, // Triangle 1 Coord A
        scale * aspect, -scale,  // Triangle 1 Coord B
        scale * aspect, scale,   // Triangle 1 Coord C
        -scale * aspect, -scale, // Triangle 2 Coord A
        scale * aspect, scale,   // Triangle 2 Coord B
        -scale * aspect, scale   // Triangle 2 Coord C
    };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    modelMatrix.identity();
    modelMatrix.Translate(position.first, position.second, 0);
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    float texCoords[] = {
        u, v + spriteY,
        u + spriteX, v + spriteY,
        u + spriteX, v,
        u, v + spriteY,
        u + spriteX, v,
        u, v
    };
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Entity::setSpriteCoords(int index)
{
    u = (float)(((int)index) % horizontalNum) / (float) horizontalNum;
    v = (float)(((int)index) / horizontalNum) / (float) verticalNum;
    spriteX = 1.0/(float)horizontalNum;
    spriteY = 1.0/(float)verticalNum;
}

Player& Player::getInstance(string& type, float xCoordinate, float yCoordinate)
{
    static Player instance(type, xCoordinate, yCoordinate);
    return instance;
}

Player::Player(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate, 9, 8, 4)
{
    textureID = LoadTexture("/Images/characters_3.png");
    kind = EntityType::Player;
    gravity = true;
}

Enemy::Enemy(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate, 0, 12, 8)
{
    textureID = LoadTexture("/Images/characters_1.png");
    if (type == "Skeleton") {
        kind = EntityType::Skeleton;
        index = 11;
        gravity = true;
    } else if (type == "Bat") {
        kind = EntityType::Bat;
        index = 53;
        gravity = false;
    } else if (type == "Spider") {
        kind = EntityType::Spider;
        index = 58;
        gravity = true;
    } else if (type == "Blob") {
        kind = EntityType::Blob;
        index = 50;
        gravity = true;
    }
}

Object::Object(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate, 0, 1, 1) {}
