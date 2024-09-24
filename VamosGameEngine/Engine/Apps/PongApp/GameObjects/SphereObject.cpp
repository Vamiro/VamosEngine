
#include "SphereObject.h"

SphereObject::SphereObject(const std::string& name)  : GameObject(name)
{
}

void SphereObject::Update()
{
    boundingSphere.Center = transform.GetPositionFloat3();
}

