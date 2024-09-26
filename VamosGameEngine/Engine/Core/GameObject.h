#pragma once
#include "Object.h"

class GameObject : public Object{
public:
    explicit GameObject(const std::string& name)
        : Object(name)
    {
    }

    virtual void Update(float deltaTime);
    virtual void Render(const SimpleMath::Matrix& viewProjectionMatrix);
};
