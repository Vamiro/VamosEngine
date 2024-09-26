#pragma once

#include <Jolt/Jolt.h>

// Определяем слои физики для вашего движка
namespace PhysicsLayers
{
    constexpr JPH::ObjectLayer STATIC = 0;    // Статические объекты (например, стены)
    constexpr JPH::ObjectLayer MOVING = 1;    // Движущиеся объекты (например, игроки)
    constexpr JPH::ObjectLayer NO_COLLISION = 2; // Объекты без коллизий

    constexpr uint32_t NUM_LAYERS = 3;        // Общее количество слоев
}

// Фильтрация слоев физики для широкофазного пересечения (BroadPhase)
class MyBroadPhaseLayer : public JPH::BroadPhaseLayerInterface
{
public:
    MyBroadPhaseLayer()
    {
        // Устанавливаем соответствие ObjectLayer и BroadPhaseLayer
        mObjectToBroadPhase[PhysicsLayers::STATIC] = JPH::BroadPhaseLayer(0);
        mObjectToBroadPhase[PhysicsLayers::MOVING] = JPH::BroadPhaseLayer(1);
        mObjectToBroadPhase[PhysicsLayers::NO_COLLISION] = JPH::BroadPhaseLayer(2);
    }

    // Возвращаем BroadPhaseLayer для объекта
    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        return mObjectToBroadPhase[inLayer];
    }

    // Возвращаем количество BroadPhase слоев
    virtual JPH::uint GetNumBroadPhaseLayers() const override
    {
        return PhysicsLayers::NUM_LAYERS;
    }

    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
        switch (inLayer.GetValue())
        {
            case PhysicsLayers::STATIC: return "STATIC";
            case PhysicsLayers::MOVING: return "MOVING";
            case PhysicsLayers::NO_COLLISION: return "NO_COLLISION";
        default: return "UNKNOWN";
        }
    }

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[PhysicsLayers::NUM_LAYERS];
};

// Фильтр для ObjectLayer -> BroadPhaseLayer
class MyObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObjectLayer, JPH::BroadPhaseLayer inBroadPhaseLayer) const override
    {
        // Логика для определения, должны ли объекты сталкиваться
        if (inObjectLayer == PhysicsLayers::NO_COLLISION)
            return false; // Игнорируем коллизии для объектов с NO_COLLISION

        // Пример: движущиеся объекты могут сталкиваться с другими объектами
        return true;
    }
};

// Фильтр для ObjectLayer -> ObjectLayer
class MyObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObjectLayer1, JPH::ObjectLayer inObjectLayer2) const override
    {
        // Определяем, должны ли два объекта сталкиваться
        if (inObjectLayer1 == PhysicsLayers::NO_COLLISION || inObjectLayer2 == PhysicsLayers::NO_COLLISION)
            return false; // Игнорируем коллизии для объектов с NO_COLLISION

        // Пример: разрешаем коллизии между всеми движущимися и статическими объектами
        return true;
    }
};
