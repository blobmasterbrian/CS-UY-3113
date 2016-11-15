//
//  gamestate.hpp
//  NYUCodebase
//
//  Created by Brian Quinn on 11/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef gamestate_hpp
#define gamestate_hpp

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "objects.hpp"

using namespace std;
enum class GameState {MainMenu, Level, Win, GameOver};

struct Level {
    Level();
    int mapWidth;
    int mapHeight;
    float tileSizeX;
    float tileSizeY;
    int xsprites;
    int ysprites;
    GLuint levelTexture;
    int** levelData;
    vector<float>* vertexData;
    vector<float>* textureData;
    vector<Entity*> entities;
    Matrix levelMatrix;
    ShaderProgram* program;
    
    void createMap();
    bool readHeader(ifstream& input);
    bool readLayerData(ifstream& input);
    bool readEntityData(ifstream& input);
    void drawMap();
    
    void placeEntity(string& type, float xCoordinate, float yCoordinate);
};

#endif /* gamestate_hpp */
