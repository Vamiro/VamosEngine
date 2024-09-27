#include "Object.h"

Object::Object(const std::string& name) : name(name)
{
    transform = new Transform(*this);
    components.push_back(transform);
}
