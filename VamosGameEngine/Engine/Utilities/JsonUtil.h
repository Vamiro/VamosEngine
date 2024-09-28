#pragma once
#include <string>
#include <fstream>

#include "ThirdParty/ModernJson/json.hpp"

namespace nlohmann
{
    template <>
    struct adl_serializer<SimpleMath::Vector3>
    {
        static void to_json(json& j, const SimpleMath::Vector3& vec)
        {
            j = json{ {"x", vec.x}, {"y", vec.y}, {"z", vec.z} };
        }

        static void from_json(const json& j, SimpleMath::Vector3& vec)
        {
            vec.x = j.at("x").get<float>();
            vec.y = j.at("y").get<float>();
            vec.z = j.at("z").get<float>();
        }
    };

    template <>
    struct adl_serializer<SimpleMath::Quaternion>
    {
        static void to_json(json& j, const SimpleMath::Quaternion& quat)
        {
            j = json{ {"x", quat.x}, {"y", quat.y}, {"z", quat.z}, {"w", quat.w} };
        }

        static void from_json(const json& j, SimpleMath::Quaternion& quat)
        {
            quat.x = j.at("x").get<float>();
            quat.y = j.at("y").get<float>();
            quat.z = j.at("z").get<float>();
            quat.w = j.at("w").get<float>();
        }
    };


}

void ReadJSON(const std::string& path, nlohmann::json& j)
{
    std::ifstream file(path);
    if (file.is_open())
    {
        file >> j;
        file.close();
    }
}

/*
{ "scenes": [
    {
    "name": "firstScene"
    "objects": [
        {
        "name": "firstObject"
        "components": [
            {
            "type": "Transform",
            "position": {"x": 0, "y": 0, "z": 0},
            "scale": {"x": 0, "y": 0, "z": 0},
            "rotation": {"x": 0, "y": 0, "z": 0, "w": 0}
          },
          {
            "type": "Model",
            "filePath": "Data\\Objects\\sphere.obj",
            "color": {"r": 0, "g": 0, "b": 0, "a": 0},
          },
          {
            "type": "ColliderComponent",
            "shape": "SphereShape",
            "position": {"x": 0, "y": 0, "z": 0},
            "scale": {"x": 0, "y": 0, "z": 0},
            "rotation": {"x": 0, "y": 0, "z": 0, "w": 0}
          }
        ]
      },
      {
        "name": "secondObject"
        "components": [

        ]
      }
    ]
  },
  {
    "name": "secondScene"
  }
]}
*/