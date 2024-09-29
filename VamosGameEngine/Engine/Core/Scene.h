#pragma once

#include <string>
#include "GameObject.h"

struct Scene {
    std::string name;
    std::vector<GameObject*> objects;
};
