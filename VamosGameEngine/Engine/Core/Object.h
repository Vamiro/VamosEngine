#pragma once
#include <string>
#include <vector>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_dx11.h>
#include <ImGUI/imgui_impl_win32.h>

#include "../Components/Transform.h"

using namespace DirectX;

class Object
{
public:
    std::string name;
    Transform* transform;

    explicit Object(const std::string& name);

    virtual void Destroy() = 0;

    template<typename T>
    T* GetComponent()
    {
        for (Component* component : components)
        {
            if (T* castedComponent = dynamic_cast<T*>(component))
            {
                return castedComponent;
            }
        }
        return nullptr;
    }

    void RenderComponents() const;

protected:
    std::vector<Component*> components;
};
