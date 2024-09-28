#pragma once

class Object;

class Component
{
public:
    Component(Object& parent, const char* name): name_(name), parent(&parent) {}
    virtual ~Component() = default;

    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void RenderGUI() = 0;

    const char* GetName() const { return name_; }
    Object& GetParent() const { return *parent; }

protected:
    const char* name_;
    Object* parent = nullptr;
};
