#pragma once

class GameObject;

class Component
{
public:
    Component(GameObject& parent, const char* name): name_(name), parent(&parent) {}
    virtual ~Component() = default;

    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void RenderGUI() = 0;

    virtual void Destroy() = 0;

    const char* GetName() const { return name_; }
    GameObject& GetParent() const { return *parent; }

protected:
    const char* name_;
    GameObject* parent = nullptr;
};