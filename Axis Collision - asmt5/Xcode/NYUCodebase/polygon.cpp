//
//  polygon.cpp
//  NYUCodebase
//
//  Created by Brian Quinn on 12/6/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "polygon.hpp"

// Polygons
Polygon::Polygon(float x, float y, int vertices): position(x,y), vertices(vertices), rotation(0.0f)//static_cast<float>(rand())/(static_cast<float>(RAND_MAX/2.0f)))
{
    coordinates = new vector<float>;
    texcoordinates = new vector<float>;
    velocity.first = -2.0f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(2.0f - (-2.0f))));
    velocity.second = -2.0f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(2.0f - (-2.0f))));
    rotationSpeed = -0.05f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(0.05f - (-0.05f))));
    scale = 1.0f + static_cast<float>(rand())/(static_cast<float>(RAND_MAX/(3.5f - 1.0f)));
}

void Polygon::update(float elapsed)
{
    modelMatrix.identity();
    position.first += velocity.first * elapsed;
    position.second += velocity.second * elapsed;
    
    if (position.first > 25.0f) {
        velocity.first = -velocity.first;
        float penetration = position.first - 24.9f;
        position.first -= penetration;
    }
    if (position.first < -25.0f) {
        velocity.first = -velocity.first;
        float penetration = position.first + 24.9f;
        position.first -= penetration;
    }
    
    if (position.second > 25.0f) {
        velocity.second = -velocity.second;
        float penetration = position.second - 24.9f;
        position.second -= penetration;
    }
    if (position.second < -25.0f) {
        velocity.second = -velocity.second;
        float penetration = position.second + 24.9f;
        position.second -= penetration;
    }
    
    rotation += rotationSpeed;
    
    modelMatrix.Translate(position.first, position.second, 0);
    modelMatrix.Rotate(rotation);
    modelMatrix.Scale(scale, scale, 1.0f);
}

void Polygon::render(ShaderProgram* program)
{
    program->setModelMatrix(modelMatrix);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, coordinates->data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texcoordinates->data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, vertices);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


// Triangles
Triangle::Triangle(float x, float y): Polygon(x, y, 6)
{
    coordinates->insert(coordinates->end(), {
        // Triangle 1
        -0.0f, -1.0f,
        -0.87f, 0.5f,
        0.87f, 0.5f
    });
    
    texcoordinates->insert(texcoordinates->end(), {
        // Triangle 1
        0.0f, 1.0f,
        0.87f, 0.5f,
        0.87f, 0.5f
    });
}

// Squares
Square::Square(float x, float y): Polygon(x, y, 12)
{
    coordinates->insert(coordinates->end(), {
        // Triangle 1
        -0.71f, 0.71f,
        -0.71f, -0.71f,
        0.71f, -0.71f,
        // Triangle 2
        -0.71f, 0.71f,
        0.71f, -0.71f,
        0.71f, 0.71f
    });
    
    texcoordinates->insert(texcoordinates->end(), {
        // Triangle 1
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        // Triangle 2
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    });
}


// Pentagons
Pentagon::Pentagon(float x, float y): Polygon(x, y, 30)
{
    coordinates->insert(coordinates->end(), {
        // Triangle 1
        0.0f, 0.0f,
        0.0f, -1.0f,
        -0.95f, -0.31f,
        // Triangle 2
        0.0f, 0.0f,
        -0.95f, -0.31f,
        -0.59f, 0.81f,
        // Triangle 3
        0.0f, 0.0f,
        -0.59f, 0.81f,
        0.59f, 0.81f,
        // Triangle 4
        0.0f, 0.0f,
        0.59f, 0.81f,
        0.95f, -0.31f,
        // Triangle 5
        0.0f, 0.0f,
        0.95f, -0.31f,
        0.0f, -1.0f
    });
    
    texcoordinates->insert(texcoordinates->end(), {
        // Triangle 1
        0.0f, 0.0f,
        0.0f, 1.0f,
        0.95f, 0.31f,
        // Triangle 2
        0.0f, 0.0f,
        0.95f, 0.31f,
        0.59f, 0.81f,
        // Triangle 3
        0.0f, 0.0f,
        0.59f, 0.81f,
        0.59f, 0.81f,
        // Triangle 4
        0.0f, 0.0f,
        0.59f, 0.81f,
        0.95f, 0.31f,
        // Triangle 5
        0.0f, 0.0f,
        0.95f, 0.31f,
        0.0f, 1.0f
    });
}


// Hexagons
Hexagon::Hexagon(float x, float y): Polygon(x, y, 36)
{
    coordinates->insert(coordinates->end(), {
        // Triangle 1
        0.0f, 0.0f,
        0.5f, -0.87f,
        -0.5f, -0.87f,
        // Triangle 2
        0.0f, 0.0f,
        -0.5f, -0.87f,
        -1.0f, 0.0f,
        // Triangle 3
        0.0f, 0.0f,
        -1.0f, 0.0f,
        -0.5f, 0.87f,
        // Triangle 4
        0.0f, 0.0f,
        -0.5f, 0.87f,
        0.5f, 0.87f,
        // Triangle 5
        0.0f, 0.0f,
        0.5f, 0.87f,
        1.0f, 0.0f,
        // Triangle 6
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, -0.87f
    });
    
    texcoordinates->insert(texcoordinates->end(), {
        // Triangle 1
        0.0f, 0.0f,
        0.5f, 0.87f,
        0.5f, 0.87f,
        // Triangle 2
        0.0f, 0.0f,
        0.5f, 0.87f,
        1.0f, 0.0f,
        // Triangle 3
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 0.87f,
        // Triangle 4
        0.0f, 0.0f,
        0.5f, 0.87f,
        0.5f, 0.87f,
        // Triangle 5
        0.0f, 0.0f,
        0.5f, 0.87f,
        1.0f, 0.0f,
        // Triangle 6
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 0.87f
    });
}




