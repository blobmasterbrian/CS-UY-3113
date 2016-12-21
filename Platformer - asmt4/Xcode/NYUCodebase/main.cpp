#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include </Library/Frameworks/SDL2_mixer.framework/Headers/SDL_mixer.h>
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

GLuint LoadFontTexture(const char* image_path)
{
    SDL_Surface* surface = IMG_Load(image_path);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
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

void drawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing, float shiftX, float shiftY) {
    float texture_size = 1.0/16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    for(int i = 0; i < text.size(); i++) {
        float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
        float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size) + shiftX, 0.5f * size + shiftY,
            ((size+spacing) * i) + (-0.5f * size) + shiftX, -0.5f * size + shiftY,
            ((size+spacing) * i) + (0.5f * size) + shiftX, 0.5f * size + shiftY,
            ((size+spacing) * i) + (0.5f * size) + shiftX, -0.5f * size + shiftY,
            ((size+spacing) * i) + (0.5f * size) + shiftX, 0.5f * size + shiftY,
            ((size+spacing) * i) + (-0.5f * size) + shiftX, -0.5f * size + shiftY,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        }); }
    
//    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

GameState state = GameState::MainMenu;

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
    GLuint background = LoadTexture("/Images/screen1.png");
    GLuint font = LoadFontTexture("/Images/font1.png");
    GLuint font2 = LoadFontTexture("/Images/font2.png");
    int konamiCode = 0;
    
    projectionMatrix.setOrthoProjection(-45.0f, 45.0f, -26.5f, 26.5f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    state = GameState::Quit;
                    done = true;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    state = GameState::Level;
                    done = true;
                }
                
                if ((konamiCode == 0 || konamiCode == 1) && event.key.keysym.scancode != SDL_SCANCODE_UP) {
                    konamiCode = 0;
                } else if ((konamiCode == 2 || konamiCode == 3) && event.key.keysym.scancode != SDL_SCANCODE_DOWN) {
                    konamiCode = 0;
                } else if ((konamiCode == 4 || konamiCode == 6) && event.key.keysym.scancode != SDL_SCANCODE_LEFT) {
                    konamiCode = 0;
                } else if ((konamiCode == 5 || konamiCode == 7) && event.key.keysym.scancode != SDL_SCANCODE_RIGHT) {
                    konamiCode = 0;
                } else if (konamiCode == 8 && event.key.keysym.scancode != SDL_SCANCODE_B) {
                    konamiCode = 0;
                } else if (konamiCode == 9 && event.key.keysym.scancode != SDL_SCANCODE_A) {
                    konamiCode = 0;
                } else {
                    ++konamiCode;
                }
            }
            if (konamiCode == 10) {
                state = GameState::Secret;
                done = true;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawBackground(&program, background, backgroundMatrix);
        drawText(&program, font2, "Regicide", 5.0f, -1.0f, -13.0f, 18.0f);
        drawText(&program, font, "Press Space to Start", 3.5f, -1.0f, -23.0f, -22.0f);
        
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
    }
    SDL_Quit();
}

void levelState(int lev)
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
//    if (state != GameState::Secret) {
//        background = LoadTexture("/Images/skybackground.png");
//    } else {
//        background = LoadTexture("/Images/secretbackground.png");
//    }
    GLuint maptex = LoadTexture("/Images/platformertiles.png");
    Level level;
    level.program = &program;
    level.levelTexture = maptex;
    level.xsprites = 8;
    level.ysprites = 5;
    
    string levelLoad[4] = {"NYUCodebase.app/Contents/Resources/Images/gamedata.txt", "NYUCodebase.app/Contents/Resources/Images/gamedata1.txt", "NYUCodebase.app/Contents/Resources/Images/gamedata2.txt", "NYUCodebase.app/Contents/Resources/Images/gamedatasecret.txt"};

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_Chunk* jump = Mix_LoadWAV("/Sounds/jump.wav");
    Mix_Music* theme = Mix_LoadMUS("/Sounds/theme.mp3");
    Mix_Music* secret = Mix_LoadMUS("/Sounds/secrettheme.mp3");
    
//createLevel:
    level.createMap(levelLoad[lev]);
    level.player->velocity.first = 0.0f;
    level.player->velocity.second = 0.0f;
//    level.player->height = 1.5;
//    level.player->width = 1.5;
//    program.setViewMatrix(level.player->playerView);
    bool changed = false;
    Mix_PlayMusic(theme, -1);
    
    while (!done) {
        
        float ticks = (float) SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (state == GameState::Secret) {
                    background = LoadTexture("/Images/secretbackground.png");
                    if (!changed) {
                        Mix_PlayMusic(secret, -1);
                        changed = true;
                    }
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    state = GameState::Quit;
                    done = true;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_UP && level.player->jump) {
                    Mix_PlayChannel(-1, jump, 0);
                    level.player->velocity.second = 15.0f;
                    level.player->jump = false;
                    level.player->index = 13;
                    if (state == GameState::Secret) {
                        level.player->index = 21;
                    }
                    level.player->setSpriteCoords(level.player->index);
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && level.player->jump) {
                    Mix_PlayChannel(-1, jump, 0);
                    level.player->velocity.second = 15.0f;
                    level.player->jump = false;
                    level.player->index = 13;
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
            if (level.entities[i] == nullptr) {
                continue;
            }
            float fixedElapsed = elapsed;
            if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
            }
            while (fixedElapsed >= FIXED_TIMESTEP) {
                fixedElapsed -= FIXED_TIMESTEP;
                level.entities[i]->update(FIXED_TIMESTEP);
                if (level.entities[i]->kind != EntityType::Player && level.entities[i]->collision(level.player)) {
                    level.player->velocity.first = 0.0f;
                    level.player->velocity.second = 0.0f;
                    state = GameState::GameOver;
                    done = true;
//                    goto end;
                }
                if (level.entities[i]->kind == EntityType::Player) {
                    int x;
                    int y;
                    worldToTileCoordinates(level.entities[i]->position.first, level.entities[i]->position.second, &x, &y);
                    if (level.levelData[y][x] == 3) {
                        done = true;
                        if (lev > 1) {
                            level.player->velocity.first = 0.0f;
                            level.player->velocity.second = 0.0f;
                            state = GameState::Win;
//                            goto end;
                        } else if (lev == 0) {
                            level.player->velocity.first = 0.0f;
                            level.player->velocity.second = 0.0f;
                            state = GameState::Level2;
                        } else if (lev == 1) {
                            level.player->velocity.first = 0.0f;
                            level.player->velocity.second = 0.0f;
                            state = GameState::Level3;
                        }
//                        goto end;
                    }
                }
            }
            level.entities[i]->update(fixedElapsed);
            if (level.entities[i]->kind != EntityType::Player && level.entities[i]->collision(level.player)) {
                level.player->velocity.first = 0.0f;
                level.player->velocity.second = 0.0f;
                state = GameState::GameOver;
                done = true;
//                goto end;
            }
            if (level.entities[i]->kind != EntityType::Player && level.entities[i]->position.second < -TILE_SIZEY*(level.mapHeight-1)) {
                delete level.entities[i];
                level.entities[i] = nullptr;
                continue;
            } else if (level.entities[i]->kind == EntityType::Player && level.entities[i]->position.second < -TILE_SIZEY*(level.mapHeight-1)) {
                level.player->velocity.first = 0.0f;
                level.player->velocity.second = 0.0f;
                state = GameState::GameOver;
                done = true;
            }
            
            
            level.entities[i]->animation += elapsed;
            if (level.entities[i]->kind == EntityType::Player && state != GameState::Secret) {
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
            } else if (level.entities[i]->kind == EntityType::Player && state == GameState::Secret) {
                if (level.entities[i]->animation > 5.0f/FPS && level.entities[i]->velocity.first > 0.5f && level.entities[i]->jump) {
                    level.entities[i]->index = ++level.entities[i]->index%8 + 16;
                    level.entities[i]->setSpriteCoords(level.entities[i]->index);
                    level.entities[i]->animation = 0.0f;
                }
                
                if (level.entities[i]->animation > 5.0f/FPS && level.entities[i]->velocity.first < -0.5f && level.entities[i]->jump) {
                    level.entities[i]->index = --level.entities[i]->index%8 + 16;
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
        
        if (state == GameState::Secret) {
            backgroundMatrix.identity();
            backgroundMatrix.Translate(level.player->position.first, level.player->position.second + 22.5f, 0.0f);
        }

        program.setViewMatrix(level.player->playerView);
        level.player->playerView.identity();
        level.player->playerView.Translate(-level.player->position.first, -level.player->position.second - 11.0f, 0.0f);
        
        if (state == GameState::Secret) {
            level.player->playerView.identity();
            level.player->playerView.Translate(-level.player->position.first, -level.player->position.second - 22.0f, 0.0f);
        }
        
//    end:
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
    }
    Mix_CloseAudio();
    Mix_FreeChunk(jump);
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
    Matrix backgroundMatrix;
    GLuint background = LoadTexture("/Images/screen3.jpg");
    GLuint font = LoadFontTexture("/Images/font2.png");
    
    projectionMatrix.setOrthoProjection(-45.0f, 45.0f, -26.5f, 26.5f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    state = GameState::Quit;
                    done = true;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_R) {
                    state = GameState::MainMenu;
                    done = true;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawBackground(&program, background, backgroundMatrix);
        drawText(&program, font, "Congratulations!", 5.5f, -2.0f, -24.5f, -20.0f);
        
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
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
    Matrix backgroundMatrix;
    GLuint background = LoadFontTexture("/Images/screen2.png");
    GLuint font = LoadFontTexture("/Images/font1.png");
    
    projectionMatrix.setOrthoProjection(-45.0f, 45.0f, -26.5f, 26.5f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    state = GameState::Quit;
                    done = true;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_R) {
                    state = GameState::MainMenu;
                    done = true;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawBackground(&program, background, backgroundMatrix);
        drawText(&program, font, "Restart?", 3.5f, -1.0f, -8.0f, -22.0f);
        
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
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
                break;
            case GameState::Level:
                levelState(0);
                break;
            case GameState::Level2:
                levelState(1);
                break;
            case GameState::Level3:
                levelState(2);
                break;
            case GameState::Secret:
                levelState(3);
                break;
            case GameState::Win:
                victory();
                break;
            case GameState::GameOver:
                gameOver();
                break;
            case GameState::Quit:
                done = true;
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    selectState();
}
