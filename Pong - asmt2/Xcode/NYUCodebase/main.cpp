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

GLuint LoadTexture(const char* image_path) {
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

struct Paddle
{
    Paddle(): x(0.0), y(0.0), height(1.0), score(0), pIndex(4), accel(0.0){}
    float x;
    float y;
    float height;
    int score;
    int pIndex;
    float accel;
};

struct Ball
{
    Ball(): x(0), y(0), bIndex(4){}
    float x;
    float y;
    int bIndex;
    float arr[2] = {0.0, 0.0};
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
    
    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    
    Matrix paddleMatrix;
    Matrix paddleMatrix2;
    Matrix ballMatrix;
    Matrix lScoreMatrix;
    Matrix rScoreMatrix;
    
    Paddle left;
    Paddle right;
    Ball ball;
    
    GLuint lPaddleTex = LoadTexture("/Images/blocks.png");
    GLuint rPaddleTex = LoadTexture("/Images/blocks.png");
    GLuint ballTex = LoadTexture("/Images/blocks.png");
    GLuint lScore = LoadTexture("/Images/pixel_font.png");
    GLuint rScore = LoadTexture("/Images/pixel_font.png");
    
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
    
    float scoreVertices[] = {
        -0.4, -0.5, // Triangle 1 Coord A
        0.4, -0.5,  // Triangle 1 Coord B
        0.4, 0.5,   // Triangle 1 Coord C
        -0.4, -0.5, // Triangle 2 Coord A
        0.4, 0.5,   // Triangle 2 Coord B
        -0.4, 0.5   // Triangle 2 Coord C
    };
    
    srand((int)time(NULL));
    
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
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    if (ball.arr[0] < 0.0){
                        ball.arr[0] -= 2.0;
                    } else {
                        ball.arr[0] += 2.0;
                    }
                    if (ball.arr[1] < 0.0){
                        ball.arr[1] -= 2.0;
                    } else {
                        ball.arr[1] += 2.0;
                    }
                    right.accel += 15.0;
                    left.accel += 15.0;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP && right.y <= 3.395) {
                    right.y += right.accel * elapsed;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_DOWN && right.y >= -3.395) {
                    right.y -= right.accel * elapsed;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_W && left.y <= 3.395) {
                    left.y += left.accel * elapsed;
                }
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_S && left.y >= -3.395) {
                    left.y -= left.accel * elapsed;
                }
            }
        }
        
        if (ball.y >= 3.895 || ball.y <= -3.895) {
            ball.arr[1] = -ball.arr[1];
            ball.bIndex = rand() % 8;
        }
        
        program.setModelMatrix(paddleMatrix);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        paddleMatrix.identity();
        paddleMatrix.Translate(-6.9, left.y, 0);
        glBindTexture(GL_TEXTURE_2D, lPaddleTex);
        int spriteCountX = 11;
        int spriteCountY = 6;
        float up1 = (float)(((int)left.pIndex) % spriteCountX) / (float) spriteCountX;
        float vp1 = (float)(((int)left.pIndex) / spriteCountX) / (float) spriteCountY;
        float spriteWidth = 1.0/(float)spriteCountX;
        float spriteHeight = 1.0/(float)spriteCountY;
        float lPaddleTexCoord[] = {
            up1, vp1+spriteHeight,
            up1+spriteWidth, vp1+spriteHeight,
            up1+spriteWidth, vp1,
            up1, vp1+spriteHeight,
            up1+spriteWidth, vp1,
            up1, vp1
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, lPaddleTexCoord);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        program.setModelMatrix(paddleMatrix2);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, paddleVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        paddleMatrix2.identity();
        paddleMatrix2.Translate(6.9, right.y, 0);
        glBindTexture(GL_TEXTURE_2D, rPaddleTex);
        float up2 = (float)(((int)right.pIndex) % spriteCountX) / (float) spriteCountX;
        float vp2 = (float)(((int)right.pIndex) / spriteCountX) / (float) spriteCountY;
        float rPaddleTexCoord[] = {
            up2, vp2+spriteHeight,
            up2+spriteWidth, vp2+spriteHeight,
            up2+spriteWidth, vp2,
            up2, vp2+spriteHeight,
            up2+spriteWidth, vp2,
            up2, vp2
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, rPaddleTexCoord);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        if (ball.x >= 6.75 && ball.x <= 7.00 && ball.y > right.y - right.height/2 && ball.y < right.y + right.height/2) {
            ball.arr[0] = -ball.arr[0];
            right.pIndex = ball.bIndex;
            ball.bIndex = rand() % 8;
        }
        if (ball.x <= -6.75 && ball.x >= -7.0 && ball.y > left.y - left.height/2 && ball.y < left.y + left.height/2) {
            ball.arr[0] = -ball.arr[0];
            left.pIndex = ball.bIndex;
            ball.bIndex = rand() % 8;
        }
        ball.x += ball.arr[0] * elapsed;
        ball.y += ball.arr[1] * elapsed;
        
        if (ball.x >= 8.5) {
            left.score++;
            ball.x = 0.0;
            ball.y = 0.0;
            if (ball.arr[0] > 2.0) {
                ball.arr[0] -= 2.0;
            }
            if (ball.arr[1] > 2.0) {
                ball.arr[1] -= 2.0;
            } else if (ball.arr[1] < -2.0) {
                ball.arr[1] += 2.0;
            }
            if (right.accel > 15.0) {
                right.accel -= 15.0;
                left.accel -= 15.0;
            }
            if (left.score == 10) {
                ball.arr[0] = 0.0;
                ball.arr[1] = 0.0;
                right.score = 0;
                right.accel = 0.0;
                left.score = 0;
                left.accel = 0.0;
            }
        }
        if (ball.x <= -8.5) {
            right.score++;
            ball.x = 0.0;
            ball.y = 0.0;
            if (ball.arr[0] < -2.0) {
                ball.arr[0] += 2.0;
            }
            if (ball.arr[1] > 2.0) {
                ball.arr[1] -= 2.0;
            } else if (ball.arr[1] < -2.0) {
                ball.arr[1] += 2.0;
            }
            if (right.accel > 15.0) {
                right.accel -= 15.0;
                left.accel -= 15.0;
            }
            if (right.score == 10) {
                ball.arr[0] = 0.0;
                ball.arr[1] = 0.0;
                left.score = 0;
                left.accel = 0.0;
                right.score = 0;
                right.accel = 0.0;
            }
        }
        
        int lIndex = left.score + 48;
        int rIndex = right.score + 48;
        
        program.setModelMatrix(ballMatrix);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ballVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        ballMatrix.identity();
        ballMatrix.Translate(ball.x, ball.y, 0);
        glBindTexture(GL_TEXTURE_2D, ballTex);
        float ub = (float)(((int)ball.bIndex) % spriteCountX) / (float) spriteCountX;
        float vb = (float)(((int)ball.bIndex) / spriteCountX) / (float) spriteCountY;
        float ballTexCoord[] = {
            ub, vb+spriteHeight,
            ub+spriteWidth, vb+spriteHeight,
            ub+spriteWidth, vb,
            ub, vb+spriteHeight,
            ub+spriteWidth, vb,
            ub, vb
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, ballTexCoord);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        program.setModelMatrix(lScoreMatrix);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, scoreVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        lScoreMatrix.identity();
        lScoreMatrix.Translate(-2.0, 3.0, 0);
        glBindTexture(GL_TEXTURE_2D, lScore);
        spriteCountX = 16;
        spriteCountY = 16;
        float u = (float)(((int)lIndex) % spriteCountX) / (float) spriteCountX;
        float v = (float)(((int)lIndex) / spriteCountX) / (float) spriteCountY;
        spriteWidth = 1.0/(float)spriteCountX;
        spriteHeight = 1.0/(float)spriteCountY;
        float texCoords[] = {
            u, v+spriteHeight,
            u+spriteWidth, v+spriteHeight,
            u+spriteWidth, v,
            u, v+spriteHeight,
            u+spriteWidth, v,
            u, v
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        program.setModelMatrix(rScoreMatrix);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, scoreVertices);
        glEnableVertexAttribArray(program.positionAttribute);
        rScoreMatrix.identity();
        rScoreMatrix.Translate(2.0, 3.0, 0);
        glBindTexture(GL_TEXTURE_2D, rScore);
        float u1 = (float)(((int)rIndex) % spriteCountX) / (float) spriteCountX;
        float v1 = (float)(((int)rIndex) / spriteCountX) / (float) spriteCountY;
        float texCoords1[] = {
            u1, v1+spriteHeight,
            u1+spriteWidth, v1+spriteHeight,
            u1+spriteWidth, v1,
            u1, v1+spriteHeight,
            u1+spriteWidth, v1,
            u1, v1
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords1);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
