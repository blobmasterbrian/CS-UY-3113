//
//  objects.cpp
//  NYUCodebase
//
//  Created by Brian Quinn on 11/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "objects.hpp"

Entity::Entity(string& type, float xCoordinate, float yCoordinate): position(xCoordinate, yCoordinate) {}

Player& Player::getInstance(string& type, float xCoordinate, float yCoordinate)
{
    static Player instance(type, xCoordinate, yCoordinate);
    return instance;
}

Player::Player(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate), horizontalNum(8), verticalNum(4), index(9)
{
    kind = EntityType::Player;
    gravity = true;
}

Enemy::Enemy(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate)
{
    if (type == "Skeleton") {
        kind = EntityType::Skeleton;
        gravity = true;
    } else if (type == "Bat") {
        kind = EntityType::Bat;
        gravity = false;
    } else if (type == "Spider") {
        kind = EntityType::Spider;
        gravity = true;
    } else if (type == "Blob") {
        kind = EntityType::Blob;
        gravity = true;
    }
}

Object::Object(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate) {}
