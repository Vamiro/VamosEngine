//
// #include "SphereObject.h"
//
// SphereObject::SphereObject(const std::string& name) : GameObject(name), sphereCollider(nullptr), model(nullptr)
// {
//     sphereCollider = new SphereCollider(*this, 1);
//     components.push_back(sphereCollider);
//
//     model = new Model(*this);
//     components.push_back(model);
// }
//
// void SphereObject::Destroy()
// {
//     delete sphereCollider;
//     delete model;
//     GameObject::Destroy();
// }
//
// void SphereObject::Update(float deltaTime)
// {
//     GameObject::Update(deltaTime);
// }
//
