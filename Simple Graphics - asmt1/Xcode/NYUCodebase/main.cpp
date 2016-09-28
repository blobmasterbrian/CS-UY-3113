#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

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
    
    GLuint redChip = LoadTexture("/Images/chipRedWhite_sideBorder.png");
    GLuint greenChip = LoadTexture("/Images/chipGreenWhite_sideBorder.png");
    GLuint blueChip = LoadTexture("/Images/chipBlueWhite_sideBorder.png");
    GLuint blackChip = LoadTexture("/Images/chipBlackWhite_sideBorder.png");
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    float dropchip1 = 2.0;
    float dropchip2 = 3.0;
    
    projectionMatrix.setOrthoProjection(-7.0f, 7.0f, -4.0f, 4.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float) SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        //            float vertices[] = {0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f};
        //            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        //            glEnableVertexAttribArray(program.positionAttribute);
        //            glDrawArrays(GL_TRIANGLES, 0, 3);
        //            glDisableVertexAttribArray(program.positionAttribute);
        
        float vertices[] = {
            // First Square
            -0.7, -0.5, // Triangle 1 Coord A
            0.7, -0.5,  // Triangle 1 Coord B
            0.7, 0.5,   // Triangle 1 Coord C
            -0.7, -0.5, // Triangle 2 Coord A
            0.7, 0.5,   // Triangle 2 Coord B
            -0.7, 0.5   // Triangle 2 Coord C
        };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glBindTexture(GL_TEXTURE_2D, redChip);
        float texCoords[] = {
            0.0, 1.0,
            1.0, 1.0,
            1.0, 0.0,
            0.0, 1.0,
            1.0, 0.0,
            0.0, 0.0
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        float vertices1[] = {
            -0.7, -0.25,
            0.7, -0.25,
            0.7, 0.75,
            -0.7, -0.25,
            0.7, 0.75,
            -0.7, 0.75
        };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glBindTexture(GL_TEXTURE_2D, greenChip);
        float texCoords1[] = {
            0.0, 1.0,
            1.0, 1.0,
            1.0, 0.0,
            0.0, 1.0,
            1.0, 0.0,
            0.0, 0.0
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords1);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        float vertices2[] = {
            -0.7, 0.0,
            0.7, 0.0,
            0.7, 1.0,
            -0.7, 0.0,
            0.7, 1.0,
            -0.7, 1.0
        };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
        glEnableVertexAttribArray(program.positionAttribute);
        
        glBindTexture(GL_TEXTURE_2D, blueChip);
        float texCoords2[] = {
            0.0, 1.0,
            1.0, 1.0,
            1.0, 0.0,
            0.0, 1.0,
            1.0, 0.0,
            0.0, 0.0
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        float vertices3[] = {
            -0.7, dropchip1,
            0.7, dropchip1,
            0.7, dropchip2,
            -0.7, dropchip1,
            0.7, dropchip2,
            -0.7, dropchip2
        };
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
        glEnableVertexAttribArray(program.positionAttribute);
        
        if (dropchip1 > 0.25) {
            dropchip1 -= elapsed * 0.5;   //0.01;
            dropchip2 -= elapsed * 0.5;   //0.01;
        } else {
            dropchip1 = 2.0;
            dropchip2 = 3.0;
        }
        glBindTexture(GL_TEXTURE_2D, blackChip);
        float texCoords3[] = {
            0.0, 1.0,
            1.0, 1.0,
            1.0, 0.0,
            0.0, 1.0,
            1.0, 0.0,
            0.0, 0.0
        };
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glDisableVertexAttribArray(program.positionAttribute);
        glDisableVertexAttribArray(program.texCoordAttribute);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
