#pragma once
#include <map>
#include <string>
#include <vector>
#include "Engine/Rendering/Mesh.h"

struct ModelBuffer {
    std::vector<std::string> fileNames;
    std::map<std::string, std::vector<Mesh>> modelMap;
};
