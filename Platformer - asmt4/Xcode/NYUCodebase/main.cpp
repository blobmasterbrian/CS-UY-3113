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

#define FPS 30.0f
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

float TILE_SIZEX;
float TILE_SIZEY;

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

void drawBackground(ShaderProgram* program, GLuint texture, Matrix matrix)
{
    program->setModelMatrix(matrix);
    float vertices[] = {
        -46.0f, 27.5f,  // Triangle 1 Coord A
        -46.0f, -27.5f, // Triangle 1 Coord B
        46.0f, -27.5f,   // Triangle 1 Coord C
        -46.0f, 27.5f,   // Triangle 2 Coord A
        46.0f, -27.5f, // Triangle 2 Coord B
        46.0f, 27.5f   // Triangle 2 Coord C
    };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glBindTexture(GL_TEXTURE_2D, texture);
    float texCoords[] = {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
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
    Matrix backgroundMatrix;
    GLuint background = LoadTexture("/Images/skybackground.png");
    
    projectionMatrix.setOrthoProjection(-45.0f, 45.0f, -26.5f, 26.5f, -1.0f, 1.0f);
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
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawBackground(&program, background, backgroundMatrix);
        
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
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
    Matrix backgroundMatrix;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    projectionMatrix.setOrthoProjection(-45.0f, 45.0f, -26.5f, 26.5f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    program.setProjectionMatrix(projectionMatrix);
    program.setViewMatrix(viewMatrix);
    
    GLuint background = LoadTexture("/Images/skybackground.png");
    GLuint maptex = LoadTexture("/Images/platformertiles.png");
    Level level;
    level.program = &program;
    level.levelTexture = maptex;
    level.xsprites = 8;
    level.ysprites = 3;
    level.createMap();
//    level.player->height = 1.5;
//    level.player->width = 1.5;
//    program.setViewMatrix(level.player->playerView);
    
    while (!done) {
        
        float ticks = (float) SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP && level.player->jump) {
                    level.player->velocity.second = 15.0f;
                    level.player->jump = false;
                    level.player->index = 13;
                    level.player->setSpriteCoords(level.player->index);
                }
            }
        }
        
        if (keys[SDL_SCANCODE_LEFT]) {
            level.player->acceleration.first -= 10.0f;
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
            level.player->acceleration.first += 10.0f;
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawBackground(&program, background, backgroundMatrix);
        level.drawMap();
//        GLuint waste = LoadTexture("/Images/characters_3.png");
//        level.player->textureID = waste;

        for (size_t i = 0; i < level.entities.size(); ++i) {
            float fixedElapsed = elapsed;
            if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
            }
            while (fixedElapsed >= FIXED_TIMESTEP) {
                fixedElapsed -= FIXED_TIMESTEP;
                level.entities[i]->update(FIXED_TIMESTEP);
            }
            level.entities[i]->update(fixedElapsed);
            
            
            level.entities[i]->animation += elapsed;
            if (level.entities[i]->kind == EntityType::Player) {
                if (level.entities[i]->animation > 5.0f/FPS && level.entities[i]->velocity.first > 0.5f && level.entities[i]->jump) {
                    level.entities[i]->index = ++level.entities[i]->index%8 + 8;
                    level.entities[i]->setSpriteCoords(level.entities[i]->index);
                    level.entities[i]->animation = 0.0f;
                }
            
                if (level.entities[i]->animation > 5.0f/FPS && level.entities[i]->velocity.first < -0.5f && level.entities[i]->jump) {
                    level.entities[i]->index = --level.entities[i]->index%8 + 8;
                    level.entities[i]->setSpriteCoords(level.entities[i]->index);
                    level.entities[i]->animation = 0.0f;
                }
            } else {
                
                if (level.entities[i]->jump) {
                    level.entities[i]->attack();
                }
                
                if (level.entities[i]->animation > 5.0f/FPS && level.entities[i]->velocity.first > 0.5f && level.entities[i]->jump) {
                    level.entities[i]->index = ++level.entities[i]->index%3 + int(level.entities[i]->kind) + 24;
                    level.entities[i]->setSpriteCoords(level.entities[i]->index);
                    level.entities[i]->animation = 0.0f;
                }
                
                if (level.entities[i]->animation > 5.0f/FPS && level.entities[i]->velocity.first < -0.5f && level.entities[i]->jump) {
                    level.entities[i]->index = --level.entities[i]->index%3 + int(level.entities[i]->kind) + 12;
                    level.entities[i]->setSpriteCoords(level.entities[i]->index);
                    level.entities[i]->animation = 0.0f;
                }
            }
            
            if (level.entities[i]->kind == EntityType::Bat && fabs(level.entities[i]->velocity.second) > 0.0f) {
                level.entities[i]->index = 51;
                level.entities[i]->setSpriteCoords(level.entities[i]->index);
            }
            
            level.entities[i]->render(&program);
        }
        
        backgroundMatrix.identity();
        backgroundMatrix.Translate(level.player->position.first, level.player->position.second + 11.0f, 0.0f);

        program.setViewMatrix(level.player->playerView);
        level.player->playerView.identity();
        level.player->playerView.Translate(-level.player->position.first, -level.player->position.second - 11.0f, 0.0f);
        
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
