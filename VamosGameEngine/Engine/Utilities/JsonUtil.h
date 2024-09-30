// #pragma once
// #include <string>
// #include <fstream>
// #include <memory>
//
// #include "Engine/Components/ColliderComponent.h"
// #include "Engine/Components/Model.h"
// #include "Engine/Core/GameObject.h"
// #include "Engine/Core/Scene.h"
// #include "ThirdParty/ModernJson/json.hpp"
//
// namespace nlohmann
// {
//     template <>
//         struct adl_serializer<SimpleMath::Vector3>
//     {
//         static void to_json(json& j, const SimpleMath::Vector3& vec)
//         {
//             j = json{ {"x", vec.x}, {"y", vec.y}, {"z", vec.z} };
//         }
//
//         static void from_json(const json& j, SimpleMath::Vector3& vec)
//         {
//             vec.x = j.at("x").get<float>();
//             vec.y = j.at("y").get<float>();
//             vec.z = j.at("z").get<float>();
//         }
//     };
//
//     template <>
//     struct adl_serializer<SimpleMath::Quaternion>
//     {
//         static void to_json(json& j, const SimpleMath::Quaternion& quat)
//         {
//             j = json{ {"x", quat.x}, {"y", quat.y}, {"z", quat.z}, {"w", quat.w} };
//         }
//
//         static void from_json(const json& j, SimpleMath::Quaternion& quat)
//         {
//             quat.x = j.at("x").get<float>();
//             quat.y = j.at("y").get<float>();
//             quat.z = j.at("z").get<float>();
//             quat.w = j.at("w").get<float>();
//         }
//     };
//
//     template <>
//     struct adl_serializer<SimpleMath::Color> {
//         static void to_json(json& j, const SimpleMath::Color& color) {
//             j = json{{"R", color.R()}, {"G", color.G()}, {"B", color.B()}, {"A", color.A()}};
//         }
//
//         static void from_json(const json& j, SimpleMath::Color& color) {
//             color = SimpleMath::Color(
//                 j.at("R").get<float>(),
//                 j.at("G").get<float>(),
//                 j.at("B").get<float>(),
//                 j.at("A").get<float>()
//                 );
//         }
//     };
//
//     template <>
//     struct adl_serializer<Transform> {
//         static void to_json(json& j, const Transform& t) {
//             j = json{{"position", t.GetPositionVector()}, {"scale", t.GetScaleVector()}, {"rotation", t.GetRotationQuaternion()}};
//         }
//
//         static void from_json(const json& j, Transform& t) {
//             t.SetPosition(j.at("position").get<SimpleMath::Vector3>());
//             t.SetScale(j.at("scale").get<SimpleMath::Vector3>());
//             t.SetRotation(j.at("rotation").get<SimpleMath::Quaternion>());
//         }
//     };
//
//     template <>
//     struct adl_serializer<Model> {
//         static void to_json(json& j, const Model& m) {
//             j = json::object();
//             j["filePath"] = m.GetModelPath();
//             j["color"] = m.GetColor();
//         }
//
//         static void from_json(const json& j, Model& m) {
//             m.SetModelPath(j.at("filePath").get<std::string>());
//             m.SetColor(j.at("color").get<SimpleMath::Color>());
//         }
//     };
//
//     template <>
//     struct adl_serializer<ColliderComponent> {
//         static void to_json(json& j, const ColliderComponent& c) {
//             j = json{{"shape", c.GetBody()->GetShape()->GetSubType()}, {"position", c.GetPositionOffset()}, {"scale", c.GetScale()}, {"rotation", c.GetRotationOffset()}};
//         }
//
//         static void from_json(const json& j, ColliderComponent& c) {
//             c.SetShape(j.at("shape").get<std::string>());
//             c.SetScale(j.at("scale").get<SimpleMath::Vector3>());
//             c.SetPositionAndRotation(j.at("position").get<SimpleMath::Vector3>(),
//                 j.at("rotation").get<SimpleMath::Quaternion>());
//         }
//     };
//
//     template <>
//     struct adl_serializer<std::unique_ptr<Component>> {
//         static void to_json(json& j, const std::unique_ptr<Component>& c) {
//             j["type"] = c->GetName();
//             if (c->GetName() == "Transform") {
//                 j["data"] = *dynamic_cast<const Transform*>(c.get());
//             } else if (c->GetName() == "Model") {
//                 j["data"] = *dynamic_cast<const Model*>(c.get());
//             } else if (c->GetName() == "ColliderComponent") {
//                 j["data"] = *dynamic_cast<const ColliderComponent*>(c.get());
//             }
//         }
//
//         static void from_json(const json& j, std::unique_ptr<Component>& c) {
//             std::string type = j.at("type").get<std::string>();
//             if (type == "Transform") {
//                 c = std::make_unique<Transform>();
//                 from_json(j.at("data"), *dynamic_cast<Transform*>(c.get()));
//             } else if (type == "Model") {
//                 c = std::make_unique<Model>();
//                 from_json(j.at("data"), *dynamic_cast<Model*>(c.get()));
//             } else if (type == "ColliderComponent") {
//                 c = std::make_unique<ColliderComponent>();
//                 from_json(j.at("data"), *dynamic_cast<ColliderComponent*>(c.get()));
//             }
//         }
//     };
//
//     template <>
//     struct adl_serializer<GameObject> {
//         static void to_json(json& j, const GameObject& obj) {
//             j = json::object();
//             j["name"] = obj.name;
//             j["components"] = json::array();
//             for (const auto& component : obj.GetComponents()) {
//                 j["components"].push_back(*component);;
//             }
//         }
//
//         static void from_json(const json& j, GameObject& obj) {
//             obj.name = j.at("name").get<std::string>();
//             for (const auto& component : j.at("components")) {
//                 std::unique_ptr<Component> comp;
//                 from_json(component, comp);
//                 obj.AddComponent(std::move(comp).release());
//             }
//         }
//     };
//
//     template <>
//     struct adl_serializer<Scene> {
//         static void to_json(json& j, const Scene& scene) {
//             j = json::object();
//             j["name"] = scene.name;
//             j["objects"] = json::array();
//             for (const auto& object : scene.objects) {
//                 j["objects"].push_back(*object);
//             }
//         }
//
//         static void from_json(const json& j, Scene& scene) {
//             scene.name = j.at("name").get<std::string>();
//             for (const auto& object : j.at("objects")) {
//                 auto gameObject = std::make_unique<GameObject>(object.at("name").get<std::string>());
//                 from_json(object, gameObject.get());
//                 scene.objects.push_back(std::move(gameObject));
//             }
//         }
//     };
// }
//
// void SaveScene(const std::string& path, const Scene& scene) {
//     nlohmann::json j = scene;
//     std::ofstream file(path);
//     if (file.is_open()) {
//         file << j.dump(4); // Pretty print with 4 spaces
//         file.close();
//     }
// }
//
// void LoadScene(const std::string& path, Scene& scene) {
//     nlohmann::json j;
//     std::ifstream file(path);
//     if (file.is_open()) {
//         file >> j;
//         file.close();
//         scene = j.get<Scene>();
//     }
// }
//
// /*
// { "scenes": [
//     {
//     "name": "firstScene"
//     "objects": [
//         {
//         "name": "firstObject"
//         "components": [
//             {
//             "type": "Transform",
//             "position": {"x": 0, "y": 0, "z": 0},
//             "scale": {"x": 0, "y": 0, "z": 0},
//             "rotation": {"x": 0, "y": 0, "z": 0, "w": 0}
//           },
//           {
//             "type": "Model",
//             "filePath": "Data\\Objects\\sphere.obj",
//             "color": {"r": 0, "g": 0, "b": 0, "a": 0},
//           },
//           {
//             "type": "ColliderComponent",
//             "shape": "SphereShape",
//             "position": {"x": 0, "y": 0, "z": 0},
//             "scale": {"x": 0, "y": 0, "z": 0},
//             "rotation": {"x": 0, "y": 0, "z": 0, "w": 0}
//           }
//         ]
//       },
//       {
//         "name": "secondObject"
//         "components": [
//
//         ]
//       }
//     ]
//   },
//   {
//     "name": "secondScene"
//   }
// ]}
// */