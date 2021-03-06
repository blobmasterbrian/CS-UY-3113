//
//  gamestate.cpp
//  NYUCodebase
//
//  Created by Brian Quinn on 11/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "gamestate.hpp"
using namespace std;

Level::Level() {vertexData = new vector<float>; textureData = new vector<float>;}

void Level::createMap(string& input)
{
    ifstream gamedata(input);
    string line;
    while (getline(gamedata, line)) {
        if (line == "[header]") {
            if (!readHeader(gamedata))
                {return;}
        } else if (line == "[layer]") {
            readLayerData(gamedata);
        } else if (line == "[ObjectsLayer]") {
            readEntityData(gamedata);
        }
    }
}

bool Level::readHeader(ifstream& input)
{
    string line;
    mapWidth = -1;
    mapHeight = -1;

    while (getline(input, line)) {
        if (line == "")
            {break;}

        istringstream sstream(line);
        string key;
        string value;
        getline(sstream, key, '=');
        getline(sstream, value);

        if (key == "width") {
            mapWidth = stoi(value);
        } else if (key == "height") {
            mapHeight = stoi(value);
        } else if (key == "tilewidth") {
            tileSizeX = stoi(value);
            TILE_SIZEX = tileSizeX * 0.1f;
        }else if (key == "tileheight") {
            tileSizeY = stoi(value);
            TILE_SIZEY = tileSizeY * 0.1f;
        }
    }

    if (mapWidth == -1 || mapHeight == -1) {
        return false;
    } else {
        levelData = new int*[mapHeight];
        for (size_t i = 0; i < mapHeight; ++i) {
            levelData[i] = new int[mapWidth];
        }
        return true;
    }
}
    
bool Level::readLayerData(ifstream& input)
{
    string line;
    while (getline(input, line)) {
        if (line == "")
            {break;}

        istringstream sstream(line);
        string key;
        string value;
        getline(sstream, key, '=');
        getline(sstream, value);

        if (key == "data") {
            for (size_t y = 0; y < mapHeight; ++y) {
                getline(input, line);
                istringstream tileStream(line);
                string tile;

                for (size_t x = 0; x < mapWidth; ++x) {
                    getline(tileStream, tile, ',');
                    int val = stoi(tile);
                    if (val > 0) {
                        levelData[y][x] = val - 1;
                    } else {
                        levelData[y][x] = -1;
                    }
                }
            }
        }
    }
    return true;
}

bool Level::readEntityData(ifstream& input)
{
    string line;
    string type;
    while (getline(input, line)) {
        if (line == "")
            {break;}

        istringstream sstream(line);
        string key;
        string value;
        getline(sstream, key, '=');
        getline(sstream, value);

        if (key == "type") {
            type = value;
        } else if (key == "location") {
            istringstream entityStream(value);
            string xPosition;
            string yPosition;
            getline(entityStream, xPosition, ',');
            getline(entityStream, yPosition, ',');

            float placeX = static_cast<float>(stoi(xPosition) * tileSizeX * 0.1f + TILE_SIZEX/2);
            float placeY = static_cast<float>(stoi(yPosition) * -tileSizeY * 0.1f - TILE_SIZEY/2);

            placeEntity(type, placeX, placeY);
        }
    }
    return true;
}

void Level::drawMap()
{
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (levelData[y][x] == -1)
                {continue;}
            float u = static_cast<float>(static_cast<int>(levelData[y][x]) % xsprites) / static_cast<float>(xsprites);
            float v = static_cast<float>(static_cast<int>(levelData[y][x]) / xsprites) / static_cast<float>(ysprites);
            float spriteWidth = 1.0f / static_cast<float>(xsprites);
            float spriteHeight = 1.0f / static_cast<float>(ysprites);
            
            vertexData->insert(vertexData->end(), {
                tileSizeX * x * 0.1f, (-tileSizeY * y - tileSizeY) * 0.1f,
                (tileSizeX * x + tileSizeX) * 0.1f, (-tileSizeY * y - tileSizeY) * 0.1f,
                (tileSizeX * x + tileSizeX) * 0.1f, -tileSizeY * y * 0.1f,
                
                tileSizeX * x * 0.1f, (-tileSizeY * y - tileSizeY) * 0.1f,
                (tileSizeX * x + tileSizeX) * 0.1f, -tileSizeY * y * 0.1f,
                tileSizeX * x * 0.1f, -tileSizeY * y * 0.1f
            });
            
            textureData->insert(textureData->end(), {
                u, v + spriteHeight,
                u + spriteWidth, v + spriteHeight,
                u + spriteWidth, v,
                
                u, v + spriteHeight,
                u + spriteWidth, v,
                u, v
            });
        }
    }
    program->setModelMatrix(levelMatrix);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData->data());
    glEnableVertexAttribArray(program->positionAttribute);
    levelMatrix.identity();
    glBindTexture(GL_TEXTURE_2D, levelTexture);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureData->data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(vertexData->size() / 2));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Level::placeEntity(string& type, float xCoordinate, float yCoordinate)
{
    Entity* entity;
    if (type == "Player") {
        player = &Player::getInstance(type, xCoordinate + 1.0f, yCoordinate - 1.25f); //new Player(type, xCoordinate, yCoordinate);
        entity = player;
        entity->scale = 1.5f;
    } else {
        entity = new Enemy(type, xCoordinate, yCoordinate);
        entity->scale = 0.75f;
    }
    entity->height = entity->scale*2;
    entity->width = entity->scale*2;
    entity->level = this;
    entities.push_back(entity);
}

void worldToTileCoordinates(float worldX, float worldY, int* gridX, int* gridY)
{
    *gridX = static_cast<int>(worldX / TILE_SIZEX);
    *gridY = static_cast<int>(-worldY / TILE_SIZEY);
}















