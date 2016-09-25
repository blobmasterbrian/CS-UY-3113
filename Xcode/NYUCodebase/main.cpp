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
    
    ShaderProgram program(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
    
    GLuint redChip = loadTexture("/Images/chipRedWhite_sideBorder.png");
    GLuint greenChip = loadTexture("/Images/chipGreenWhite_sideBorder.png");
    GLuint blueChip = loadTexture("/Images/chipBlueWhite_sideBorder.png");
    GLuint blackChip = loadTexture("/Images/chipBlackWhite_sideBorder.png");
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    float dropchip1 = 2.0;
    float dropchip2 = 3.0;
    
    projectionMatrix.setOrthoProjection(-7.0f, 7.0f, -4.0f, 4.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
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
            
            float vertices1[] = {
                -0.7, 0.0,
                0.7, 0.0,
                0.7, 1.0,
                -0.7, 0.0,
                0.7, 1.0,
                -0.7, 1.0
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
            
            float vertices2[] = {
                -0.7, 0.5,
                0.7, 0.5,
                0.7, 1.5,
                -0.7, 0.5,
                0.7, 1.5,
                -0.7, 1.5
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
            
            if (dropchip1 > 1.5) {
                dropchip1 -= 0.01;
                dropchip2 -= 0.01;
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
            
            glDrawArrays(GL_TRIANGLES, 0, 24);
            glDisableVertexAttribArray(program.positionAttribute);
            glDisableVertexAttribArray(program.texCoordAttribute);
            
            SDL_GL_SwapWindow(displayWindow);
            
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
