//
//  objects.hpp
//  NYUCodebase
//
//  Created by Brian Quinn on 11/13/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef objects_hpp
#define objects_hpp

#include <stdio.h>
#include "ShaderProgram.h"

enum class EntityType {Player, Enemy, Object, Platform, Intangible};

struct Entity
{
    void update(float elapsed);
    void render(ShaderProgram* program);
    bool collision(Entity* entity);
    
    
};

#endif /* objects_hpp */
