#pragma once
#include "Object.h"

class GameObject : public Object{
public:
    explicit GameObject(const std::string& name, bool visible = true)
        : Object(name), _isVisible(visible)
    {
    }

    void RenderComponentsGUI();

    virtual void Start();
    virtual void Update(float deltaTime);
    void Render(const SimpleMath::Matrix& viewProjectionMatrix);
    void Destroy() override;

    [[nodiscard]] bool IsVisible() const { return _isVisible; }
    void SetVisible(const bool visible) { _isVisible = visible; }

private:
    bool _isVisible;
};
