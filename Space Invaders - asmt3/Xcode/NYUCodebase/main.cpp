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
    Vec2D(float x = 0.0f, float y = 0.0f): x(x), y(y) {}
    float x;
    float y;
};

struct texturedObject
{
    texturedObject(): u(0.0f), v(0.0f), height(1.0f), width(1.0f) {}
    texturedObject(GLuint id, float height = 1.0f, float width = 1.0f, Vec2D position = Vec2D(0.0f, 0.0f), float scale = 1.0f):
    textureID(id), u(0.0f), v(0.0f), height(height), width(width), spriteX(1.0f), spriteY(1.0f), position(position), scale(scale){}
    
    void setPosition(float x, float y)
    {
        position.x = x;
        position.y = y;
    }
    
    void update(float elapsed)
    {
        if (velocity.x < 10.0f) {
            velocity.x += acceleration.x * elapsed;
        }
        if (velocity.y < 10.0f) {
            velocity.y += acceleration.y * elapsed;
        }
        position.x += velocity.x * elapsed;
        position.y += velocity.y * elapsed;
    }
    
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
    float scale;
};

struct spriteSheet : texturedObject
{
    spriteSheet(GLuint id, int xElements, int yElements, int index = 1, float height = 1.0f, float width = 1.0f, float scale = 1.0f):
    texturedObject(id, height, width, Vec2D(0.0f, 0.0f), scale), index(index), horizontalNum(xElements), verticalNum(yElements)
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

void drawTexture(ShaderProgram* program, texturedObject* obj) //, float scale = 1.0f)
{
    program->setModelMatrix(obj->matrix);
    float aspect = obj->width/obj->height;
    float vertices[] = {
        -0.5f * obj->scale * aspect, -0.5f * obj->scale, // Triangle 1 Coord A
        0.5f * obj->scale * aspect, -0.5f * obj->scale,  // Triangle 1 Coord B
        0.5f * obj->scale * aspect, 0.5f * obj->scale,   // Triangle 1 Coord C
        -0.5f * obj->scale * aspect, -0.5f * obj->scale, // Triangle 2 Coord A
        0.5f * obj->scale * aspect, 0.5f * obj->scale,   // Triangle 2 Coord B
        -0.5f * obj->scale * aspect, 0.5f * obj->scale   // Triangle 2 Coord C
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

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
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
    texturedObject obj(i, 1.0f, 4.0f, Vec2D(0.0f,0.0f), 7.0f);
    
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
        drawTexture(&program, &obj); //, 7.0f);
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
    
    GLuint ships = LoadTexture("/Images/1942.png");
    spriteSheet player(ships, 17, 31, 102);
    player.setPosition(0.0f, -7.0f);
    
    GLuint text = LoadTexture("/Images/pixel_font.png");
    spriteSheet ones(text, 16, 16, 48, 1.5f);
    ones.setPosition(13.0f, 7.0f);
    spriteSheet tens(text, 16, 16, 48, 1.5f);
    tens.setPosition(12.4f, 7.0f);
    spriteSheet huns(text, 16, 16, 48, 1.5f);
    huns.setPosition(11.8f, 7.0f);
    spriteSheet thou(text, 16, 16, 48, 1.5f);
    thou.setPosition(11.2f, 7.0f);
    
    std::vector<spriteSheet*> score;
    score.push_back(&ones);
    score.push_back(&tens);
    score.push_back(&huns);
    score.push_back(&thou);
    
    int q[15] = {15,15,15,15,15,15,15,15,15,15,15,15,15,15,15};
    std::vector<int> frontmost[15];
    std::vector<int> rowmost[6];
    
//    int rightmost[] = {0,15,30,45,60,75};
//    int leftmost[] = {14,29,44,59,74,89};
//    int frontmost[] = {75,76,77,78,79,80,81,82,83,84,85,86,87,88,89};
    
    std::vector<spriteSheet*> enemies;
    float j = 10.5;
    int enemyShip = rand() % 3 + 112; // 85,86 = pow
    for (int i = 0; i < 15; i++) {
        spriteSheet* enemy = new spriteSheet(ships, 17, 31, enemyShip, 1.0f, 1.0f, 1.3f);
        enemy->setPosition(j-=1.5, 7.0f);
        enemy->velocity.x = -2.0f;
        enemy->velocity.y = 0.0f;
        enemies.push_back(enemy);
        frontmost[i].push_back(int(enemies.size()-1));
        rowmost[0].push_back(i);
        enemyShip = rand() % 3 + 112;
    }
    j = 10.5;
    enemyShip = rand() % 3 + 112;
    for (int i = 0; i < 15; i++) {
        spriteSheet* enemy = new spriteSheet(ships, 17, 31, enemyShip, 1.0f, 1.0f, 1.3f);
        enemy->setPosition(j-=1.5, 6.0f);
        enemy->velocity.x = -2.0f;
        enemy->velocity.y = 0.0f;
        enemies.push_back(enemy);
        frontmost[i].push_back(int(enemies.size()-1));
        rowmost[1].push_back(i+15);
        enemyShip = rand() % 3 + 112;
    }
    j = 10.5;
    enemyShip = rand() % 3 + 112;
    for (int i = 0; i < 15; i++) {
        spriteSheet* enemy = new spriteSheet(ships, 17, 31, enemyShip, 1.0f, 1.0f, 1.3f);
        enemy->setPosition(j-=1.5, 5.0f);
        enemy->velocity.x = -2.0f;
        enemy->velocity.y = 0.0f;
        enemies.push_back(enemy);
        frontmost[i].push_back(int(enemies.size()-1));
        rowmost[2].push_back(i+30);
        enemyShip = rand() % 3 + 112;
    }
    j = 10.5;
    enemyShip = rand() % 3 + 112;
    for (int i = 0; i < 15; i++) {
        spriteSheet* enemy = new spriteSheet(ships, 17, 31, enemyShip, 1.0f, 1.0f, 1.3f);
        enemy->setPosition(j-=1.5, 4.0f);
        enemy->velocity.x = -2.0f;
        enemy->velocity.y = 0.0f;
        enemies.push_back(enemy);
        frontmost[i].push_back(int(enemies.size()-1));
        rowmost[3].push_back(i+45);
        enemyShip = rand() % 3 + 112;
    }
    j = 10.5;
    enemyShip = rand() % 3 + 112;
    for (int i = 0; i < 15; i++) {
        spriteSheet* enemy = new spriteSheet(ships, 17, 31, enemyShip, 1.0f, 1.0f, 1.3f);
        enemy->setPosition(j-=1.5, 3.0f);
        enemy->velocity.x = -2.0f;
        enemy->velocity.y = 0.0f;
        enemies.push_back(enemy);
        frontmost[i].push_back(int(enemies.size()-1));
        rowmost[4].push_back(i+60);
        enemyShip = rand() % 3 + 112;
    }
    j = 10.5;
    enemyShip = rand() % 3 + 112;
    for (int i = 0; i < 15; i++) {
        spriteSheet* enemy = new spriteSheet(ships, 17, 31, enemyShip, 1.0f, 1.0f, 1.3f);
        enemy->setPosition(j-=1.5, 2.0f);
        enemy->velocity.x = -2.0f;
        enemy->velocity.y = 0.0f;
        enemies.push_back(enemy);
        frontmost[i].push_back(int(enemies.size()-1));
        rowmost[5].push_back(i+75);
        enemyShip = rand() % 3 + 112;
    }
    
    GLuint lazer = LoadTexture("/Images/laserBlue01.png");
    GLuint enemyLazer = LoadTexture("/Images/laserRed01.png");
    std::vector<texturedObject*> playerBullets;
    std::vector<texturedObject*> enemyBullets;
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    projectionMatrix.setOrthoProjection(-14.0f, 14.0f, -8.0f, 8.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    texturedObject* bullet = new texturedObject(lazer, 1.0f, 0.1f, Vec2D(player.position.x + 0.15f, player.position.y + 0.3f), 0.5f);
                    bullet->acceleration.y = 50.0f;
                    playerBullets.push_back(bullet);
                }
            }
        }
        
        if (keys[SDL_SCANCODE_LEFT]) {
                player.acceleration.x = -50.0f;
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
                player.acceleration.x = 50.0f;
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        float ticks = (float) SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        player.velocity.x = lerp(player.velocity.x, 0.0f, elapsed * 5.0f);
        player.acceleration.x = lerp(player.acceleration.x, 0.0f, elapsed * 0.5f);
        player.update(elapsed);
        drawTexture(&program, &player);
        
        for (size_t k = 0; k < score.size(); k++) {
            drawTexture(&program, score[k]);
        }
        
        for (int k = 0; k < enemies.size(); k++) {
            if (enemies[k] != nullptr) {
                if (enemies[rowmost[k/15][rowmost[k/15].size()-1]]->position.x <= -13.5f || enemies[rowmost[k/15][0]]->position.x >= 10.0f) {
                    float movement = 10.0f + (16.0f - rowmost[k/15].size());
                    enemies[k]->velocity.x = -enemies[k]->velocity.x;
                    enemies[k]->position.y -= movement * elapsed;
                }
                enemies[k]->update(elapsed);
                drawTexture(&program, enemies[k]); //, 1.3f);
                if (enemies[k]->index == 85 || enemies[k]->index == 86) {
                    std::vector<int>::iterator begin = frontmost[k%15].begin();
                    std::vector<int>::iterator end = frontmost[k%15].end();
                    std::vector<int>::iterator eraser = std::remove(begin, end, k);
                    frontmost[k % q[k%15]].erase(eraser);
                    rowmost[k/15].erase(std::remove(rowmost[k/15].begin(), rowmost[k/15].end(), k));
                    delete enemies[k];
                    enemies[k] = nullptr;
                }
            }
        }
        std::cout << *q << std::endl;
        for (size_t k = 0; k < 14; k++) {
            int shoot = rand();
            if (shoot % 263 == 0 && frontmost[k].size() > 0) {
                Vec2D vec(enemies[*(frontmost[k].end()-1)]->position.x - 0.15f,
                          enemies[*(frontmost[k].end()-1)]->position.y -
                          enemies[*(frontmost[k].end()-1)]->height*enemies[*(frontmost[k].end()-1)]->scale/2.0f);
                texturedObject* shot = new texturedObject(enemyLazer, 1.0f, 0.1f, vec, 0.5f);
                shot->acceleration.y = -40.0f;
                enemyBullets.push_back(shot);
            }
        }
        
        for (size_t k = 0; k < playerBullets.size(); k++) {
            if (playerBullets[k] != nullptr) {
                playerBullets[k]->velocity.x = lerp(playerBullets[k]->velocity.x, 0.0f, elapsed * 5.0f);
                playerBullets[k]->acceleration.x = lerp(playerBullets[k]->acceleration.x, 0.0f, elapsed * 0.5f);
                playerBullets[k]->update(elapsed);
                drawTexture(&program, playerBullets[k]); //, 0.5f);
            }
        }
        for (size_t k = 0; k < enemyBullets.size(); k++) {
            enemyBullets[k]->velocity.x = lerp(enemyBullets[k]->velocity.x, 0.0f, elapsed * 5.0f);
            enemyBullets[k]->acceleration.x = lerp(enemyBullets[k]->acceleration.x, 0.0f, elapsed * 0.5f);
            enemyBullets[k]->update(elapsed);
            drawTexture(&program, enemyBullets[k]); //, 0.5f);
        }
        
        for (size_t k = 0; k < enemies.size(); k++) {
            if (enemies[k] != nullptr) {
                for (size_t l = 0; l < playerBullets.size(); l++) {
                    if (playerBullets[l] != nullptr) {
                        if(playerBullets[l]->position.y + playerBullets[l]->height*playerBullets[l]->scale/2.0f >   // Bullet Top
                           enemies[k]->position.y - enemies[k]->height*enemies[k]->scale/2.0f &&                    // Enemy Bottom
                           playerBullets[l]->position.x + playerBullets[l]->width*playerBullets[l]->scale/2.0f >    // Bullet Right
                           enemies[k]->position.x - enemies[k]->width*enemies[k]->scale/2.0f &&                     // Enemy Left
                           playerBullets[l]->position.x - playerBullets[l]->width*playerBullets[l]->scale/2.0f <    // Bullet Left
                           enemies[k]->position.x + enemies[k]->width*enemies[k]->scale/2.0f &&                     // Enemy Right
                           playerBullets[l]->position.y - playerBullets[l]->height*playerBullets[l]->scale/2.0f <   // Bullet Bottom
                           enemies[k]->position.y + enemies[k]->height*enemies[k]->scale/2.0f) {                    // Enemy Top
                        
                            delete playerBullets[l];
                            playerBullets[l] = nullptr;
                        
                            int newIdx = rand() % 2;
                            enemies[k]->index = newIdx + 85;
                            enemies[k]->setSpriteCoords(newIdx + 85);
                            drawTexture(&program, enemies[k]);
                        
                            if (score[0]->index == 57) {
                                if (score[1]->index == 57) {
                                    if (score[2]->index == 57) {
                                        score[3]->setSpriteCoords(++score[3]->index);
                                        score[2]->index = 48;
                                        score[2]->setSpriteCoords(48);
                                    } else {
                                        score[2]->setSpriteCoords(++score[2]->index);
                                    }
                                    score[1]->index = 48;
                                    score[1]->setSpriteCoords(48);
                                } else {
                                    score[1]->setSpriteCoords(++score[1]->index);
                                }
                                score[0]->index = 48;
                                score[0]->setSpriteCoords(48);
                            } else {
                                score[0]->setSpriteCoords(++score[0]->index);
                            }
                        }
                    }
                }
            }
        }
        
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
