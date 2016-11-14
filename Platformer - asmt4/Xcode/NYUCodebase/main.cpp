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
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    bool done = false;
    glViewport(0, 0, 1280, 720);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    
    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    state = GameState::Level;
                    done = true;
                }
            }
        glClear(GL_COLOR_BUFFER_BIT);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
        }
    }
    SDL_Quit();
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
    
    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
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
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
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
        level.drawMap();
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
    }
    SDL_Quit();
}

void victory()
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
    glViewport(0, 0, 1280, 720);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    
    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    done = true;
                }
            }
            glClear(GL_COLOR_BUFFER_BIT);
            program.setProjectionMatrix(projectionMatrix);
            program.setViewMatrix(viewMatrix);
            glDisableVertexAttribArray(program.positionAttribute);
            glDisableVertexAttribArray(program.texCoordAttribute);
            SDL_GL_SwapWindow(displayWindow);
        }
    }
    SDL_Quit();
}

void gameOver()
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
    glViewport(0, 0, 1280, 720);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    
    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    done = true;
                }
            }
            glClear(GL_COLOR_BUFFER_BIT);
            program.setProjectionMatrix(projectionMatrix);
            program.setViewMatrix(viewMatrix);
            glDisableVertexAttribArray(program.positionAttribute);
            glDisableVertexAttribArray(program.texCoordAttribute);
            SDL_GL_SwapWindow(displayWindow);
        }
    }
    SDL_Quit();
}

void selectState()
{
    bool done = false;
    while (!done) {
        switch (state) {
            case GameState::MainMenu:
                mainMenu();
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
