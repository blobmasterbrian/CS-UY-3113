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

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

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

struct Vec2D
{
    Vec2D(float x = 0.0f, float y = -0.0f): x(x), y(y) {}
    float x;
    float y;
};

struct texturedObject
{
    texturedObject(): u(0.0f), v(0.0f), height(1.0f), width(1.0f) {}
    texturedObject(GLuint id, float height = 1.0f, float width = 1.0f): textureID(id), u(0.0f), v(0.0f),
                                                                        height(height), width(width), spriteX(1.0f), spriteY(1.0f){}
    GLuint textureID;
    Matrix matrix;
    float u;
    float v;
    float height;
    float width;
    float spriteX;
    float spriteY;
    Vec2D position;
    Vec2D velocity;
    Vec2D acceleration;
};

struct spriteSheet : texturedObject
{
    spriteSheet(GLuint id, int xElements, int yElements, int index = 1, float height = 1.0f, float width = 1.0f): texturedObject(id, height, width)
    , index(index), horizontalNum(xElements), verticalNum(yElements)
    {
        u = (float)(((int)index) % horizontalNum) / (float) horizontalNum;
        v = (float)(((int)index) / horizontalNum) / (float) verticalNum;
        spriteX = 1.0/(float)horizontalNum;
        spriteY = 1.0/(float)verticalNum;
    }
    
    void setSpriteCoords(int index)
    {
        u = (float)(((int)index) % horizontalNum) / (float) horizontalNum;
        v = (float)(((int)index) / horizontalNum) / (float) verticalNum;
        spriteX = 1.0/(float)horizontalNum;
        spriteY = 1.0/(float)verticalNum;
    }
    int index;
    int horizontalNum;
    int verticalNum;
};

void drawTexture(ShaderProgram* program, texturedObject* obj, float scale = 1.0f)
{
    program->setModelMatrix(obj->matrix);
    float aspect = obj->width/obj->height;
    float vertices[] = {
        -0.5f * scale * aspect, -0.5f * scale, // Triangle 1 Coord A
        0.5f * scale * aspect, -0.5f * scale,  // Triangle 1 Coord B
        0.5f * scale * aspect, 0.5f * scale,   // Triangle 1 Coord C
        -0.5f * scale * aspect, -0.5f * scale, // Triangle 2 Coord A
        0.5f * scale * aspect, 0.5f * scale,   // Triangle 2 Coord B
        -0.5f * scale * aspect, 0.5f * scale   // Triangle 2 Coord C
    };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    obj->matrix.identity();
    obj->matrix.Translate(obj->position.x, obj->position.y, 0);
    
    glBindTexture(GL_TEXTURE_2D, obj->textureID);
    float texCoords[] = {
        obj->u, obj->v + obj->spriteY,
        obj->u + obj->spriteX, obj->v + obj->spriteY,
        obj->u + obj->spriteX, obj->v,
        obj->u, obj->v + obj->spriteY,
        obj->u + obj->spriteX, obj->v,
        obj->u, obj->v
    };
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

enum class EntityType {Player, Platform, Enemy};
enum class GameState {MainMenu, Level};
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
    
    GLuint i = LoadTexture("/Images/menu.png");
    texturedObject obj(i, 1.0f, 4.0f);
    
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
        drawTexture(&program, &obj, 7.0f);
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
    
    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    
    GLuint i = LoadTexture("/Images/1942.png");
    spriteSheet obj(i, 17, 31, 102);
    
    srand((int)time(NULL));
    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        float ticks = (float) SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        drawTexture(&program, &obj);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
}

void selectState()
{
    const Uint8* key = SDL_GetKeyboardState(NULL);
    if (key[SDL_SCANCODE_SPACE]) {
        state = GameState::Level;
    }
    switch (state) {
        case GameState::MainMenu:
            mainMenu();
            
        case GameState::Level:
            levelState();
            break;
    }
}

int main(int argc, char *argv[])
{
    selectState();
}
