#pragma once
#include <map>
#include <string>
#include <vector>
#include "Engine/Rendering/Texture.h"

struct TextureBuffer {
    std::vector<std::string> fileNames;
    std::map<std::string, std::vector<Texture>> textureMap;
};
