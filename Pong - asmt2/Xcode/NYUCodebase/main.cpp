#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <random>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char* image_path) {
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

struct Paddle
{
    Paddle(): x(0), y(0) {}
    float x;
    float y;
};

struct Ball
{
    Ball(): x(0), y(0) {}
    float x;
    float y;
    float arr[2] = {1.5, 1.5};
};

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    bool done = false;
    float lastFrameTicks = 0.0f;
    glViewport(0, 0, 640, 360);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ShaderProgram program(RESOURCE_FOLDER "vertex.glsl", RESOURCE_FOLDER "fragment.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    
    Matrix paddleMatrix;
    Matrix paddleMatrix2;
    Matrix ballMatrix;
    
    Paddle left;
    Paddle right;
    Ball ball;
    
    float paddleVertices[] = {
        -0.1, -0.5, // Triangle 1 Coord A
        0.1, -0.5,  // Triangle 1 Coord B
        0.1, 0.5,   // Triangle 1 Coord C
        -0.1, -0.5, // Triangle 2 Coord A
        0.1, 0.5,   // Triangle 2 Coord B
        -0.1, 0.5   // Triangle 2 Coord C
    };
    
    float ballVertices[] = {
        -0.1, -0.1, // Triangle 1 Coord A
        0.1, -0.1,  // Triangle 1 Coord B
        0.1, 0.1,   // Triangle 1 Coord C
        -0.1, -0.1, // Triangle 2 Coord A
        0.1, 0.1,   // Triangle 2 Coord B
        -0.1, 0.1   // Triangle 2 Coord C
    };
    
    projectionMatrix.setOrthoProjection(-7.0f, 7.0f, -4.0f, 4.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float) SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    right.y += 15.0 * elapsed;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    right.y -= 15.0 * elapsed;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_W) {
                    left.y += 15.0 * elapsed;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_S) {
                    left.y -= 15.0 * elapsed;
                }
            }
        }
        
        if (ball.y >= 3.895 || ball.y <= -3.895) {
            ball.arr[1] = -ball.arr[1];
        }
        
        ball.x += ball.arr[0] * elapsed;
        ball.y += ball.arr[1] * elapsed;
        
        program.setModelMatrix(paddleMatrix);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        paddleMatrix.identity();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        paddleMatrix.Translate(-6.9, left.y, 0);
        
        program.setModelMatrix(paddleMatrix2);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        paddleMatrix2.identity();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        paddleMatrix2.Translate(6.9, right.y, 0);
        
        program.setModelMatrix(ballMatrix);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        ballMatrix.identity();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        ballMatrix.Translate(ball.x, ball.y, 0);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
