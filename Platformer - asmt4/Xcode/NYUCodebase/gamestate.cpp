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

void Level::createMap()
{
    ifstream gamedata("NYUCodebase.app/Contents/Resources/Images/gamedata.txt");
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
        }else if (key == "tileheight") {
            tileSizeY = stoi(value);
        }
    }

    if (mapWidth == -1 || mapHeight == -1) {
        return false;
    } else {
        levelData = new unsigned char*[mapHeight];
        for (size_t i = 0; i < mapHeight; ++i) {
            levelData[i] = new unsigned char[mapWidth];
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
                    unsigned char val = static_cast<unsigned char>(stoi(tile));
                    if (val > 0) {
                        levelData[y][x] = val - 1;
                    } else {
                        levelData[y][x] = 0;
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

            float placeX = static_cast<float>(stoi(xPosition) * tileSizeX);
            float placeY = static_cast<float>(stoi(yPosition) * -tileSizeY);

            placeEntity(type, placeX, placeY);
        }
    }
    return true;
}

void Level::drawMap()
{
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (levelData[y][x] == 0)
                {continue;}
            float u = static_cast<float>((static_cast<int>(levelData[y][x]) % xsprites) / static_cast<float>(xsprites));
            float v = static_cast<float>((static_cast<int>(levelData[y][x]) / xsprites) / static_cast<float>(ysprites));
            float spriteWidth = 1.0f / static_cast<float>(xsprites);
            float spriteHeight = 1.0f / static_cast<float>(ysprites);
            

            vertexData->insert(vertexData->end(), {
                static_cast<float>(tileSizeX * x * 1), static_cast<float>(-tileSizeY * y * 1),
                static_cast<float>(tileSizeX * x * 1), static_cast<float>(((-tileSizeY * y) - tileSizeY) * 1),
                static_cast<float>(((tileSizeX * x) + tileSizeX) * 1), static_cast<float>(((-tileSizeY * y) - tileSizeY) * 1),
                
                static_cast<float>(tileSizeX * x * 1), static_cast<float>(-tileSizeY * y * 1),
                static_cast<float>(((tileSizeX * x) + tileSizeX) * 1), static_cast<float>(((-tileSizeY * y) - tileSizeY) * 1),
                static_cast<float>(((tileSizeX * x) + tileSizeX) * 1), static_cast<float>(-tileSizeY * y * 1)
            });
            
            textureData->insert(textureData->end(), {
                u, v,
                u, v + spriteHeight,
                u + spriteWidth, v + spriteHeight,
                
                u, v,
                u + spriteWidth, v + spriteHeight,
                u + spriteWidth, v
            });
        }
    }
    program->setModelMatrix(levelMatrix);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData);
    glEnableVertexAttribArray(program->positionAttribute);
    levelMatrix.identity();
    glBindTexture(GL_TEXTURE_2D, levelTexture);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, textureData);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(vertexData->size() / 2));
    
//    program->setModelMatrix(levelMatrix);
//    float vertices[] = {
//        -0.5f * 2, -0.5f * 2, // Triangle 1 Coord A
//        0.5f * 2, -0.5f * 2,  // Triangle 1 Coord B
//        0.5f * 2, 0.5f * 2,   // Triangle 1 Coord C
//        -0.5f * 2, -0.5f * 2, // Triangle 2 Coord A
//        0.5f * 2, 0.5f * 2,   // Triangle 2 Coord B
//        -0.5f * 2, 0.5f * 2   // Triangle 2 Coord C
//    };
//    
//    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
//    glEnableVertexAttribArray(program->positionAttribute);
//    levelMatrix.identity();
//    
//    float u = static_cast<float>((static_cast<int>(3) % xsprites) / static_cast<float>(xsprites));
//    float v = static_cast<float>((static_cast<int>(3) / xsprites) / static_cast<float>(ysprites));
//    float spriteWidth = 1.0f / static_cast<float>(xsprites);
//    float spriteHeight = 1.0f / static_cast<float>(ysprites);
//    
//    glBindTexture(GL_TEXTURE_2D, levelTexture);
//    float texCoords[] = {
//        u, v + spriteHeight,
//        u + spriteWidth, v + spriteHeight,
//        u + spriteWidth, v,
//        
//        u, v + spriteHeight,
//        u + spriteWidth, v,
//        u, v
//    };
//    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
//    glEnableVertexAttribArray(program->texCoordAttribute);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void placeEntity(string& type, float xCoordinate, float yCoordinate)
{
    
}

















