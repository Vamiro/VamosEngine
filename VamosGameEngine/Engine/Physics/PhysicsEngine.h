#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <Jolt/Jolt.h>
#include <Jolt/Math/Real.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include "Jolt/RegisterTypes.h"
#include "Jolt/Renderer/DebugRenderer.h"
#include <cstdarg>


using namespace JPH::literals;

void TraceImpl(const char *inFMT, ...);

#ifdef JPH_ENABLE_ASSERTS
bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, JPH::uint inLine);
#endif // JPH_ENABLE_ASSERTS

namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
    static constexpr JPH::ObjectLayer MOVING = 1;
    static constexpr JPH::ObjectLayer PLAYER = 2;
    static constexpr JPH::ObjectLayer NUM_LAYERS = 3;
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
};

namespace BroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr JPH::BroadPhaseLayer PLAYER(2);
    static constexpr JPH::uint NUM_LAYERS(3);
};

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl();
    virtual JPH::uint GetNumBroadPhaseLayers() const override;
    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
};

class MyContactListener : public JPH::ContactListener
{
public:
    virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override;
    virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
    virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
    virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;
};

class MyBodyActivationListener : public JPH::BodyActivationListener
{
public:
    virtual void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override;
    virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override;
};

class MyDebugRenderer : public JPH::DebugRenderer {
public:
    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
    virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
    virtual Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
    virtual Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override;
    virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) override;
    virtual void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override;
};

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();
    void Initialize(float gravityScale = 1.0f);
    void UpdatePhysics(float deltaTime);
    JPH::PhysicsSystem* GetPhysicsSystem();
    JPH::DebugRenderer* GetDebugRenderer() const;
    void Cleanup();

private:
    static constexpr JPH::uint cMaxBodies = 1024;
    static constexpr JPH::uint cNumBodyMutexes = 0;
    static constexpr JPH::uint cMaxBodyPairs = 1024;
    static constexpr JPH::uint cMaxContactConstraints = 1024;
    const int cCollisionSteps = 1;
    const float cDeltaTime = 1.0f / 120.0f;

    JPH::TempAllocatorImpl* temp_allocator;
    JPH::JobSystemThreadPool* job_system;
    JPH::PhysicsSystem physics_system;
    MyBodyActivationListener body_activation_listener;
    MyContactListener contact_listener;
    BPLayerInterfaceImpl broad_phase_layer_interface;
    ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;
    ObjectLayerPairFilterImpl object_vs_object_layer_filter;
    JPH::BodyInterface& body_interface = physics_system.GetBodyInterface();
    JPH::DebugRenderer* debug_renderer;
};

#endif // PHYSICS_ENGINE_H