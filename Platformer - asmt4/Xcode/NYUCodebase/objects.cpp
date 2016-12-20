//
//  objects.cpp
//  NYUCodebase
//
//  Created by Brian Quinn on 11/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "objects.hpp"

Entity::Entity(string& type, float xCoordinate, float yCoordinate, int idx, int xSprites, int ySprites, float scale): index(idx), horizontalNum(xSprites), verticalNum(ySprites), scale(scale), position(xCoordinate, yCoordinate), animation(0.0f)
{
    u = (float)(((int)index) % horizontalNum) / (float) horizontalNum;
    v = (float)(((int)index) / horizontalNum) / (float) verticalNum;
    spriteX = 1.0/(float)horizontalNum;
    spriteY = 1.0/(float)verticalNum;
}

void Entity::update(float elapsed)
{
    modelMatrix.identity();
    position.first += velocity.first * elapsed;
    position.second += velocity.second * elapsed;
    
    jump = resolveCollision();
    if (jump && kind != EntityType::Bat) {
        acceleration.second = 0.0f;
    }
    
    if (velocity.first > -10.0f && velocity.first < 10.0f) {
        velocity.first += acceleration.first * elapsed;
    }
    if (velocity.second > -10.0f) {
        velocity.second += acceleration.second * elapsed;
    }
    
    if (friction) {
        velocity.first = lerp(velocity.first, 0.0f, elapsed);
    }

    acceleration.first = lerp(acceleration.first, 0.0f, elapsed * 10.0f);
    if (gravity && !jump) {
        if (kind != EntityType::Blob) {
            velocity.second = lerp(velocity.second, -10.0f, elapsed);
        }
        acceleration.second = lerp(acceleration.second, -50.0f, elapsed);
    }
    
//    jump = resolveCollision();
//    if (jump) {
//        acceleration.second = 0.0f;
//    }
    
    modelMatrix.Translate(position.first, position.second, 0);
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
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

bool Entity::resolveCollision()
{
    int top;
    int top2;
    int left;
    int left2;
    int right;
    int right2;
    int bottom;
    int bottom2;
    
    worldToTileCoordinates(position.first - width/2*0.5f, position.second, &left, &left2);
    worldToTileCoordinates(position.first + width/2*0.5f, position.second, &right, &right2);
//    worldToTileCoordinates(position.first, position.second, &gridPosition.first, &gridPosition.second);
    
    // right and left
    switch (level->levelData[left2][left]) {
        case -1:
        case 3:
        case 11:
        case 13:
        case 14:
        case 15:
        case 21:
        case 22:
        case 23:
            break;
        default:
            if (kind == EntityType::Player) {
                velocity.first = 0.0f;
            } else if (kind == EntityType::Spider) {
                gravity = false;
                velocity.second = 2.0f;
            } else {
                velocity.first = -velocity.first;
            }
            position.first += fabs(TILE_SIZEX*(left+1) - (position.first-width/2*0.51f));
    }
    switch (level->levelData[right2][right]) {
        case -1:
        case 3:
        case 11:
        case 13:
        case 14:
        case 15:
        case 21:
        case 22:
        case 23:
            break;
        default:
            if (kind == EntityType::Player) {
                velocity.first = 0.0f;
            } else if (kind == EntityType::Spider) {
                gravity = false;
                velocity.second = 2.0f;
            } else {
                velocity.first = -velocity.first;
            }
            position.first -= fabs(TILE_SIZEX*right - (position.first+width/2*0.51f));
    }
    
    
    // top
    worldToTileCoordinates(position.first + width/2*0.5f, position.second+height/2*0.5f, &top, &top2);
    switch (level->levelData[top2][top]) {
        case -1:
        case 3:
        case 11:
        case 13:
        case 14:
        case 15:
        case 21:
        case 22:
        case 23:
//            if (kind == EntityType::Spider) {
//                gravity = true;
//            }
            break;
        default:
            if (kind == EntityType::Spider) {
                gravity = false;
                velocity.first = -velocity.first;
            }
            velocity.second = 0.0f;
            position.second -= fabs(-TILE_SIZEY*(top2+1) - (position.second+height/2*0.5f));
    }
    worldToTileCoordinates(position.first - width/2*0.5f, position.second+height/2*0.5f, &top, &top2);
    switch (level->levelData[top2][top]) {
        case -1:
        case 3:
        case 11:
        case 13:
        case 14:
        case 15:
        case 21:
        case 22:
        case 23:
//            if (kind == EntityType::Spider) {
//                gravity = true;
//            }
            break;
        default:
            if (kind == EntityType::Spider) {
                gravity = false;
                velocity.first = -velocity.first;
            }
            velocity.second = 0.0f;
            position.second -= fabs(-TILE_SIZEY*(top2+1) - (position.second+height/2*0.5f));
    }
    
    if (kind == EntityType::Spider) {
        worldToTileCoordinates(position.first + width/2*0.5f, position.second+height/2*0.5f + 0.25f, &top, &top2);
        switch (level->levelData[top2][top]) {
            case -1:
            case 3:
            case 11:
            case 13:
            case 14:
            case 15:
            case 21:
            case 22:
            case 23:
                gravity = true;
                break;
            default:
                break;
        }
        worldToTileCoordinates(position.first - width/2*0.5f, position.second+height/2*0.5f + 0.25f, &top, &top2);
        switch (level->levelData[top2][top]) {
            case -1:
            case 3:
            case 11:
            case 13:
            case 14:
            case 15:
            case 21:
            case 22:
            case 23:
                gravity = true;
                break;
            default:
                return true;
                break;
        }
    }
    
    
    // bottom
    worldToTileCoordinates(position.first + width/2*0.5f, position.second - height/2, &bottom, &bottom2);
    switch (level->levelData[bottom2][bottom]) {
        case -1:
        case 3:
        case 11:
        case 13:
        case 14:
        case 15:
        case 21:
        case 22:
        case 23:
            break;
        default:
            position.second += fabs(-TILE_SIZEY*bottom2 - (position.second-height/2));
//            velocity.second = 0.0f;
            return true;
    }
//    if (level->levelData[bottom2][bottom] != -1) {  // bottom
//        velocity.second = 0.0f;
//        return true;
//    }
    worldToTileCoordinates(position.first - width/2*0.5f, position.second - height/2, &bottom, &bottom2);
    switch (level->levelData[bottom2][bottom]) {
        case -1:
        case 3:
        case 11:
        case 13:
        case 14:
        case 15:
        case 21:
        case 22:
        case 23:
            break;
        default:
            position.second += fabs(-TILE_SIZEY*bottom2 - (position.second-height/2));
//            velocity.second = 0.0f;
            return true;
    }
//    if (level->levelData[bottom2][bottom] != -1) {  // bottom
//        velocity.second = 0.0f;
//        return true;
//    }
    if (kind == EntityType::Bat) {
        return true;
    }
    
    return false;
}

bool Entity::collision(Entity* entity)
{
    if (position.first + width/2*0.5f < entity->position.first - entity->width/2*0.5f || position.first - width/2*0.5f > entity->position.first + entity->width/2*0.5f || position.second + width/2*0.5f < entity->position.second - entity->width/2*0.5f || position.second - width/2*0.5f > entity->position.second + entity->width/2*0.5f) {
        return false;
    }
    return true;
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
    instance.position.first = xCoordinate;
    instance.position.second = yCoordinate;
    instance.velocity.first = 0.0f;
    instance.velocity.second = 0.0f;
    instance.textureID = LoadTexture("/Images/characters_3.png");
    return instance;
}

Player::Player(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate, 9, 8, 4)
{
    textureID = LoadTexture("/Images/characters_3.png");
    kind = EntityType::Player;
    gravity = true;
    friction = true;
}

void Player::attack() {
    
}

Enemy::Enemy(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate, 0, 12, 8)
{
    textureID = LoadTexture("/Images/characters_1.png");
    if (type == "Skeleton") {
        kind = EntityType::Skeleton;
        index = 10;
        setSpriteCoords(index);
        gravity = true;
    } else if (type == "Bat") {
        kind = EntityType::Bat;
        index = 53;
        setSpriteCoords(index);
        gravity = false;
    } else if (type == "Spider") {
        kind = EntityType::Spider;
        index = 58;
        setSpriteCoords(index);
        gravity = true;
    } else if (type == "Blob") {
        kind = EntityType::Blob;
        index = 50;
        setSpriteCoords(index);
        gravity = true;
    }
    
    velocity.first = 1.0f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(2.5f - (1.0f))));
    if (rand()%2 == 0) {
        velocity.first = -velocity.first;
    }
    
    friction = false;
    initHeight = position.second;
    initSpeed = velocity.first;
}

void Enemy::attack()
{
    int attack = rand();
    if (kind == EntityType::Skeleton) {
        if (attack % 281 == 0) {
            index = 10;
            setSpriteCoords(index);
            velocity.first = 0.0f;
        } else if (velocity.first == 0.0f && attack % 53 == 0) {
            velocity.first = 1.0f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(5.0f - (1.0f))));
            if (rand()%2 == 0) {
                velocity.first = -velocity.first;
            }
        }
    } else if (kind == EntityType::Bat) {
        if (attack % 281 == 0 && position.second < initHeight && position.second > initHeight - 1.5f) {
            index = 51;
            setSpriteCoords(index);
            
            if (velocity.first > 0.0f) {
                velocity.first += 1.5f;
            } else {
                velocity.first -= 1.5f;
            }
            
            velocity.second += -4.0f;
            acceleration.second = 3.0f;
        } else if (position.second >= initHeight) {
            if (fabs(velocity.first) > fabs(initSpeed)) {
                if (velocity.first > 0.0f) {
                    velocity.first -= 1.5f;
                } else {
                    velocity.first += 1.5f;
                }
            }
            acceleration.second = 0.0f;
            velocity.second = 0.0f;
            position.second = initHeight - 0.1f;
        }
    } else if (kind == EntityType::Spider) {
        if (attack % 281 == 0) {
            position.second -= 0.25f;
            gravity = true;
        }
    } else if (kind == EntityType::Blob) {
        if (attack % 199 == 0) {
            index = 49;
            setSpriteCoords(index);
            velocity.first = 0.0f;
        } else if (velocity.first == 0.0f && attack % 19 == 0) {
            acceleration.second = 20.0f;
            acceleration.first = 20.0f;
            velocity.first = 2.5f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(5.0f - (2.5f))));
            velocity.second = 3.5f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(5.0f - (3.5f))));
            if (rand()%2 == 0) {
                velocity.first = -velocity.first;
                if (velocity.first < 0.0f) {
                    index = 62;
                } else {
                    index = 74;
                }
                setSpriteCoords(index);
            } else {
                if (velocity.first < 0.0f) {
                    index = 62;
                } else {
                    index = 74;
                }
                setSpriteCoords(index);
            }
        } else if (fabs(velocity.first) > fabs(initSpeed)) {
            if (velocity.first > 0.0f) {
                velocity.first = fabs(initSpeed);
            } else {
                velocity.first = -fabs(initSpeed);
            }
        }
    }
}

Object::Object(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate, 0, 1, 1) {}
void Object::attack() {}










