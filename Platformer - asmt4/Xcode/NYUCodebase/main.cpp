#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <random>
#include "time.h"
#include <vector>
#include <unordered_map>
#include <list>
#include "objects.hpp"
#include "gamestate.hpp"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

SDL_Window* displayWindow;

GLuint LoadTexture(const char* image_path)
{
    SDL_Surface* surface = IMG_Load(image_path);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    SDL_FreeSurface(surface);

    return textureID;
}

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

GameState state = GameState::Level;

void mainMenu()
{
//    SDL_Init(SDL_INIT_VIDEO);
//    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
//    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
//    SDL_GL_MakeCurrent(displayWindow, context);
//#ifdef _WINDOWS
//    glewInit();
//#endif
//    
//    SDL_Event event;
//    bool done = false;
//    glViewport(0, 0, 1280, 720);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    
//    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
//    
//    Matrix projectionMatrix;
//    Matrix viewMatrix;
//    
//    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
//    glUseProgram(program.programID);
//    
//    while (!done) {
//        while (SDL_PollEvent(&event)) {
//            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
//                done = true;
//            }
//            if (event.type == SDL_KEYDOWN) {
//                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
//                    state = GameState::Level;
//                    done = true;
//                }
//            }
//        glClear(GL_COLOR_BUFFER_BIT);
//        program.setProjectionMatrix(projectionMatrix);
//        program.setViewMatrix(viewMatrix);
//        glDisableVertexAttribArray(program.positionAttribute);
//        glDisableVertexAttribArray(program.texCoordAttribute);
//        SDL_GL_SwapWindow(displayWindow);
//        }
//    }
//    SDL_Quit();
}

void levelState()
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    bool done = false;
    float lastFrameTicks = 0.0f;
    glViewport(0, 0, 1280, 720);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    srand((int)time(NULL));
    
    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    projectionMatrix.setOrthoProjection(-45.0f, 45.0f, -26.5f, 26.5f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        
        float ticks = (float) SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                }
            }
        }
        
        if (keys[SDL_SCANCODE_LEFT]) {
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        GLuint maptex = LoadTexture("/Images/platformertiles.png");
        Level level;
        level.program = &program;
        level.levelTexture = maptex;
        level.xsprites = 8;
        level.ysprites = 3;
        level.createMap();
//        program.setViewMatrix(level.player->playerView);
        level.drawMap();
        
        for (size_t i = 0; i < level.entities.size(); ++i) {
            float fixedElapsed = elapsed;
            if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
            }
            while (fixedElapsed >= FIXED_TIMESTEP) {
                fixedElapsed -= FIXED_TIMESTEP;
//                level.entities[i]->update(FIXED_TIMESTEP);
            }
//            level.entities[i]->update(fixedElapsed);
            level.entities[i]->render(&program);
        }
        
        Matrix mtx;
        GLuint tex = LoadTexture("/Images/characters_3.png");
        
        program.setModelMatrix(mtx);
        float scale = level.player->scale;
        float aspect = level.player->width/level.player->height;
        float u = level.player->u;
        float v = level.player->v;
        float spriteX = level.player->spriteX;
        float spriteY = level.player->spriteY;
        
        float vertices[] = {
            -1.5f, -1.5f, // Triangle 1 Coord A
            1.5f, -1.5f,  // Triangle 1 Coord B
            1.5f, 1.5f,   // Triangle 1 Coord C
            -1.5f, -1.5f, // Triangle 2 Coord A
            1.5f, 1.5f,   // Triangle 2 Coord B
            -1.5f, 1.5f   // Triangle 2 Coord C
        };
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        mtx.identity();
        mtx.Translate(10.0f, 10.0f, 0);
        
        glBindTexture(GL_TEXTURE_2D, tex);
        float texCoords[] = {
            u, v + spriteY,
            u + spriteX, v + spriteY,
            u + spriteX, v,
            u, v + spriteY,
            u + spriteX, v,
            u, v
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        viewMatrix.identity();
//        viewMatrix.Translate(-level.player->position.first, -level.player->position.second, 0.0f);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
    }
    SDL_Quit();
}

void victory()
{
//    SDL_Init(SDL_INIT_VIDEO);
//    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
//    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
//    SDL_GL_MakeCurrent(displayWindow, context);
//#ifdef _WINDOWS
//    glewInit();
//#endif
//    
//    SDL_Event event;
//    bool done = false;
//    glViewport(0, 0, 1280, 720);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    
//    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
//    
//    Matrix projectionMatrix;
//    Matrix viewMatrix;
//    
//    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
//    glUseProgram(program.programID);
//    
//    while (!done) {
//        while (SDL_PollEvent(&event)) {
//            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
//                done = true;
//            }
//            if (event.type == SDL_KEYDOWN) {
//                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
//                    done = true;
//                }
//            }
//            glClear(GL_COLOR_BUFFER_BIT);
//            program.setProjectionMatrix(projectionMatrix);
//            program.setViewMatrix(viewMatrix);
//            glDisableVertexAttribArray(program.positionAttribute);
//            glDisableVertexAttribArray(program.texCoordAttribute);
//            SDL_GL_SwapWindow(displayWindow);
//        }
//    }
//    SDL_Quit();
}

void gameOver()
{
//    SDL_Init(SDL_INIT_VIDEO);
//    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
//    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
//    SDL_GL_MakeCurrent(displayWindow, context);
//#ifdef _WINDOWS
//    glewInit();
//#endif
//    
//    SDL_Event event;
//    bool done = false;
//    glViewport(0, 0, 1280, 720);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    
//    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
//    
//    Matrix projectionMatrix;
//    Matrix viewMatrix;
//    
//    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
//    glUseProgram(program.programID);
//    
//    while (!done) {
//        while (SDL_PollEvent(&event)) {
//            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
//                done = true;
//            }
//            if (event.type == SDL_KEYDOWN) {
//                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
//                    done = true;
//                }
//            }
//            glClear(GL_COLOR_BUFFER_BIT);
//            program.setProjectionMatrix(projectionMatrix);
//            program.setViewMatrix(viewMatrix);
//            glDisableVertexAttribArray(program.positionAttribute);
//            glDisableVertexAttribArray(program.texCoordAttribute);
//            SDL_GL_SwapWindow(displayWindow);
//        }
//    }
//    SDL_Quit();
}

void selectState()
{
    bool done = false;
    while (!done) {
        switch (state) {
            case GameState::MainMenu:
                mainMenu();
                break;
            case GameState::Level:
                levelState();
                break;
            case GameState::Win:
                victory();
                done = true;
                break;
            case GameState::GameOver:
                gameOver();
                done = true;
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    selectState();
}
