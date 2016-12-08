#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "polygon.hpp"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

pair<float,float> modelToWorldCoordinates(const Matrix& modelMatrix, const pair<float,float>& modelCoordinates)
{
    pair<float,float> worldCoordinates;
    worldCoordinates.first = modelMatrix.m[0][0] * modelCoordinates.first + modelMatrix.m[0][1] * modelCoordinates.second + modelMatrix.m[2][0] * 1.0f + modelMatrix.m[3][0] * 1.0f;
    worldCoordinates.second = modelMatrix.m[1][0] * modelCoordinates.first + modelMatrix.m[1][1] * modelCoordinates.second + modelMatrix.m[2][1] * 1.0f + modelMatrix.m[3][1] * 1.0f;
    return worldCoordinates;
}

bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<pair<float,float>>& points1, const std::vector<pair<float,float>>& points2) {
    float normalX = -edgeY;
    float normalY = edgeX;
    float len = sqrtf(normalX*normalX + normalY*normalY);
    normalX /= len;
    normalY /= len;
    
    std::vector<float> e1Projected;
    std::vector<float> e2Projected;
    
    for(int i=0; i < points1.size(); i++) {
        e1Projected.push_back(points1[i].first * normalX + points1[i].second * normalY);
    }
    for(int i=0; i < points2.size(); i++) {
        e2Projected.push_back(points2[i].first * normalX + points2[i].second * normalY);
    }
    
    std::sort(e1Projected.begin(), e1Projected.end());
    std::sort(e2Projected.begin(), e2Projected.end());
    
    float e1Min = e1Projected[0];
    float e1Max = e1Projected[e1Projected.size()-1];
    float e2Min = e2Projected[0];
    float e2Max = e2Projected[e2Projected.size()-1];
    float e1Width = fabs(e1Max-e1Min);
    float e2Width = fabs(e2Max-e2Min);
    float e1Center = e1Min + (e1Width/2.0);
    float e2Center = e2Min + (e2Width/2.0);
    float dist = fabs(e1Center-e2Center);
    float p = dist - ((e1Width+e2Width)/2.0);
    
    if(p < 0) {
        return true;
    }
    return false;
}

bool checkSATCollision(const std::vector<pair<float,float>>& e1Points, const std::vector<pair<float,float>>& e2Points) {
    for(int i=0; i < e1Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e1Points.size()-1) {
            edgeX = e1Points[0].first - e1Points[i].first;
            edgeY = e1Points[0].second - e1Points[i].second;
        } else {
            edgeX = e1Points[i+1].first - e1Points[i].first;
            edgeY = e1Points[i+1].second - e1Points[i].second;
        }
        
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
        if(!result) {
            return false;
        }
    }
    for(int i=0; i < e2Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e2Points.size()-1) {
            edgeX = e2Points[0].first - e2Points[i].first;
            edgeY = e2Points[0].second - e2Points[i].second;
        } else {
            edgeX = e2Points[i+1].first - e2Points[i].first;
            edgeY = e2Points[i+1].second - e2Points[i].second;
        }
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
        if(!result) {
            return false;
        }
    }
    return true;
}

bool checkCollision(Polygon& object1, Polygon& object2)
{
    float worldX;
    float worldY;
    vector<pair<float,float>> edges1;
    vector<pair<float,float>> edges2;
    for (size_t i = 0; i < object1.coordinates->size(); i += 2) {
        if ((*object1.coordinates)[i] == 0.0f && (*object1.coordinates)[i+1] == 0.0f)
            {continue;}
        
//        if ((*object1.coordinates)[i] > object1.position.first) {
//             + object1.scale;
//        } else if ((*object1.coordinates)[i] < object1.position.first) {
//            worldX = (*object1.coordinates)[i] + object1.position.first - object1.scale;
//        }
//        if ((*object1.coordinates)[i+1] > object1.position.second) {
//            worldY = (*object1.coordinates)[i+1] + object1.position.second + object1.scale;
//        } else if ((*object1.coordinates)[i+1] < object1.position.second) {
//            worldY = (*object1.coordinates)[i+1] + object1.position.second - object1.scale;
//        }
        worldX = (*object1.coordinates)[i] + object1.position.first;
        worldY = (*object1.coordinates)[i+1] + object1.position.second;
//        edges1.push_back(pair<float,float>(worldX, worldY));
        
        pair<float,float> coordinatePair((*object1.coordinates)[i], (*object1.coordinates)[i+1]);
        edges1.push_back(modelToWorldCoordinates(object1.modelMatrix, coordinatePair));
    }
    for (size_t i = 0; i < object2.coordinates->size(); i += 2) {
        if ((*object2.coordinates)[i] == 0.0f && (*object2.coordinates)[i+1] == 0.0f)
            {continue;}
//        if ((*object2.coordinates)[i] > object2.position.first) {
//            worldX = (*object2.coordinates)[i] + object2.position.first + object2.scale;
//        } else if ((*object2.coordinates)[i] < object2.position.first) {
//            worldX = (*object2.coordinates)[i] + object2.position.first - object2.scale;
//        }
//        if ((*object2.coordinates)[i+1] > object2.position.second) {
//            worldY = (*object2.coordinates)[i+1] + object2.position.second + object2.scale;
//        } else if ((*object2.coordinates)[i+1] < object2.position.second) {
//            worldY = (*object2.coordinates)[i+1] + object2.position.second - object2.scale;
//        }
        worldX = (*object2.coordinates)[i] + object2.position.first;
        worldY = (*object2.coordinates)[i+1] + object2.position.second;
//        edges2.push_back(pair<float,float>(worldX, worldY));
        
        pair<float,float> coordinatePair((*object2.coordinates)[i], (*object2.coordinates)[i+1]);
        pair<float,float> pair2 = modelToWorldCoordinates(object2.modelMatrix, coordinatePair);
        edges2.push_back(pair2);
    }
    return checkSATCollision(edges1, edges2);
//    return true;
}

void normalize(pair<float,float>& norm)
{
    float length = sqrtf(norm.first * norm.first + norm.second * norm.second);
    norm.first /= length;
    norm.second /= length;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    SDL_Event event;
    bool done = false;
    float lastFrameTicks = 0.0f;
    glViewport(0, 0, 700, 600);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ShaderProgram program(RESOURCE_FOLDER "vertex.glsl", RESOURCE_FOLDER "fragment.glsl");
    
    Matrix projectionMatrix;
    Matrix viewMatrix;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    srand(static_cast<unsigned>(time(NULL)));
    
    int numShapes = 10;
    vector<Polygon*> shapes;
    for (size_t i = 0; i < numShapes; ++i) {
        int gen = rand()%4 + 3;
        float placeX = -24.0f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(24.0f - (-24.0f))));
        float placeY = -24.0f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(24.0f - (-24.0f))));
        Polygon* polygon;
        switch (gen) {
            case 3:
                polygon = new Triangle(placeX, placeY);
                break;
            case 4:
                polygon = new Square(placeX, placeY);
                break;
            case 5:
                polygon = new Pentagon(placeX, placeY);
                break;
            case 6:
                polygon = new Hexagon(placeX, placeY);
                break;
        }
        shapes.push_back(polygon);
    }
    
    projectionMatrix.setOrthoProjection(-25.0f, 25.0f, -25.0f, 25.0f, -25.0f, 25.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        
        if (keys[SDL_SCANCODE_UP]) {
        }
        if (keys[SDL_SCANCODE_LEFT]) {
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float) SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        for (size_t i = 0; i < shapes.size(); ++i) {
            shapes[i]->update(elapsed);
            
            for (size_t k = i+1 % shapes.size(); k < shapes.size(); ++k) {
                int maxChecks = 100;
                while(checkCollision(*shapes[i], *shapes[k]) && maxChecks > 0) {
                    pair<float,float> response(shapes[k]->position.first - shapes[i]->position.first, shapes[k]->position.second - shapes[i]->position.second);;
                    normalize(response);
                    
                    shapes[i]->position.first -= response.first * 0.002f;
                    shapes[i]->position.second -= response.second * 0.002f;
                    shapes[k]->position.first += response.first * 0.002f;
                    shapes[k]->position.second += response.second * 0.002f;
                    --maxChecks;
                }
            }
            shapes[i]->render(&program);
        }
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
