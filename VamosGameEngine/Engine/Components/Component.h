#pragma once
#include <string>

class Object;

class Component
{
    public:
        Component(Object& parent, const char* name): name_(name) { this->parent = &parent; } ;
        virtual void RenderGUI() = 0;
        Object& GetParent() const { return *parent; }
        const char* GetName() const { return name_; }

    protected:
        Object* parent = nullptr;
        const char* name_;
};
