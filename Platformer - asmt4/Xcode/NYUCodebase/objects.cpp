//
//  objects.cpp
//  NYUCodebase
//
//  Created by Brian Quinn on 11/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "objects.hpp"

Entity::Entity(string& type, float xCoordinate, float yCoordinate) {}

Player& Player::getInstance(string& type, float xCoordinate, float yCoordinate)
{
    static Player instance(type, xCoordinate, yCoordinate);
    return instance;
}

Player::Player(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate), horizontalNum(8), verticalNum(4), index(9) {kind = EntityType::Player;}

Enemy::Enemy(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate)
{
    
}

Object::Object(string& type, float xCoordinate, float yCoordinate): Entity(type, xCoordinate, yCoordinate) {}
