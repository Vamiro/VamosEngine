#pragma once
#include "Engine/Rendering/GameObject.h"

class BoxObject : public GameObject {
public:
    explicit BoxObject(const std::string& name);

    [[nodiscard]] BoundingBox GetBoundingBox() const { return boundingBox; }
    void SetBoundingCenter(XMFLOAT3 value) { boundingBox.Center = value; }
    void SetBoundingBoxExtention(const SimpleMath::Vector3 extents) { boundingBox.Extents = extents; }

private:
    BoundingBox boundingBox;
};
