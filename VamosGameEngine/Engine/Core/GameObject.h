#pragma once
#include "Object.h"

class GameObject : public Object{
public:
    explicit GameObject(const std::string& name)
        : Object(name)
    {
    }

    virtual void Start();
    virtual void Update(float deltaTime);
    void Render(const SimpleMath::Matrix& viewProjectionMatrix);
    void Destroy() override;
};
