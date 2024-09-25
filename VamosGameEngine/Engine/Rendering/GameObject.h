#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

#include "Engine/Components/Transform.h"

//TODO: Use simple math library and quaternions

using namespace DirectX;

class GameObject
{
public:
    std::string name;

    explicit GameObject(const std::string& name);

    virtual void Update() = 0;
    virtual void Render() = 0;

    Transform transform;


    template<typename T>
    T* GetComponent()
    {
        for (IComponent* component : components)
        {
            if (T* castedComponent = dynamic_cast<T*>(component))
            {
                return castedComponent;
            }
        }
        return nullptr;
    }

protected:
    std::vector<IComponent*> components;
};
