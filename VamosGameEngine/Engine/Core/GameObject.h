#pragma once
#include <string>
#include <vector>
#include <SimpleMath.h>
#include "Engine/Utilities/ImGuiHelper.h"
#include "Engine/Utilities/JsonUtil.h"

class Component;
class Transform;

class GameObject {
public:
    explicit GameObject(const std::string& name, bool visible = true);

    void RenderComponentsGUI();

    virtual void Start();
    virtual void Update(float deltaTime);
    void Render(const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projectionMatrix,
                DirectX::SimpleMath::Vector3 lightDirection);
    virtual void Destroy();
    [[nodiscard]] int GetId() const { return id; }

    [[nodiscard]] bool IsVisible() const { return _isVisible; }
    void SetVisible(const bool visible) { _isVisible = visible; }

    [[nodiscard]] GameObject* GetParent() const { return _parent; }
    void SetParent(GameObject* parent);
    [[nodiscard]] const std::vector<GameObject*>& GetChildren() const { return _children; }
    void AddChild(GameObject* child) { _children.push_back(child); }


    void AddComponent(Component* component){ _components.push_back(component); }
    void DeleteComponent(Component* component);
    [[nodiscard]] std::vector<Component*> GetComponents() const { return _components; }

    void RenderComponentsGUI() const;


    std::string name;
    Transform* transform;

protected:
    static int nextId;
    const int id;

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
