#pragma once
#include <string>
#include <vector>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_dx11.h>
#include <ImGUI/imgui_impl_win32.h>

#include "SimpleMath.h"
#include "Engine/Components/Component.h"
#include "Engine/Utilities/JsonUtil.h"

class Component;
class Transform;
class ColliderComponent;
class Model;

using namespace DirectX;

class GameObject {
public:
    explicit GameObject(const std::string& name, bool visible = true);

    void RenderComponentsGUI();

    virtual void Start();
    virtual void Update(float deltaTime);
    void Render(const SimpleMath::Matrix& viewProjectionMatrix);
    virtual void Destroy();

    [[nodiscard]] bool IsVisible() const { return _isVisible; }
    void SetVisible(const bool visible) { _isVisible = visible; }

    [[nodiscard]] GameObject* GetParent() const { return _parent; }
    void SetParent(GameObject* parent);
    [[nodiscard]] const std::vector<GameObject*>& GetChildren() const { return _children; }
    void AddChild(GameObject* child) { _children.push_back(child); }


    void AddComponent(Component* component){ _components.push_back(component); }
    void DeleteComponent(Component* component)
    {
        std::erase(_components, component);
    }
    [[nodiscard]] std::vector<Component*> GetComponents() const { return _components; }

    void RenderComponentsGUI() const;

    std::string name;
    Transform* transform;

protected:
    bool _isVisible;
    GameObject* _parent = nullptr;
    std::vector<GameObject*> _children;
    std::vector<Component*> _components;

public:
    template<typename T>
T* GetComponent()
    {
        for (Component* component : _components)
        {
            if (T* castedComponent = dynamic_cast<T*>(component))
            {
                return castedComponent;
            }
        }
        return nullptr;
    }
};
