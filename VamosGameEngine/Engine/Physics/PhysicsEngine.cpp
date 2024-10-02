#include "PhysicsEngine.h"
#include <DirectXMath.h>
#include <iostream>
#include <SimpleMath.h>

#include "Delegate.h"
#include "Engine/Components/ColliderComponent.h"


static void TraceImpl(const char *inFMT, ...)
{
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    // Print to the TTY
    std::cout << buffer << std::endl;
}

#ifdef JPH_ENABLE_ASSERTS
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, JPH::uint inLine)
{
    // Print to the TTY
    std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr? inMessage : "") << std::endl;

    // Breakpoint
    return true;
}
#endif // JPH_ENABLE_ASSERTS

bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
    switch (inObject1)
    {
    case Layers::NON_MOVING:
        return inObject2 == Layers::MOVING; // Non moving only collides with moving
    case Layers::MOVING:
        return true; // Moving collides with everything
    default:
        JPH_ASSERT(false);
        return false;
    }
}

BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
    // Create a mapping table from object to broad phase layer
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
}

JPH::uint BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
    return BroadPhaseLayers::NUM_LAYERS;
}

JPH::BroadPhaseLayer BPLayerInterfaceImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
{
    JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    return mObjectToBroadPhase[inLayer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char *BPLayerInterfaceImpl::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
{
    switch ((JPH::BroadPhaseLayer::Type)inLayer)
    {
    case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
    case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING: return "MOVING";
    default: JPH_ASSERT(false); return "INVALID";
    }
}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
    switch (inLayer1)
    {
    case Layers::NON_MOVING:
        return inLayer2 == BroadPhaseLayers::MOVING;
    case Layers::MOVING:
        return true;
    default:
        JPH_ASSERT(false);
        return false;
    }
}

JPH::ValidateResult MyContactListener::OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult)
{
    //std::cout << "Contact validate callback" << std::endl;
    // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
    return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void MyContactListener::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
    auto* collider1 = reinterpret_cast<ColliderComponent*>(inBody1.GetUserData());
    auto* collider2 = reinterpret_cast<ColliderComponent*>(inBody2.GetUserData());

    if (collider1 && collider2) {
        collider1->OnCollision().Execute(collider2);
        collider2->OnCollision().Execute(collider1);
    }
    //std::cout << "A contact was added" << std::endl;
}

void MyContactListener::OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
    //std::cout << "A contact was persisted" << std::endl;
}

void MyContactListener::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair)
{
    //std::cout << "A contact was removed" << std::endl;
}

void MyBodyActivationListener::OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData)
{
    //std::cout << "A body got activated" << std::endl;
}

void MyBodyActivationListener::OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData)
{
    //std::cout << "A body went to sleep" << std::endl;
}

void MyDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
}

void MyDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor,
    ECastShadow inCastShadow)
{
}

JPH::DebugRenderer::Batch MyDebugRenderer::CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount)
{
    return nullptr;
}

JPH::DebugRenderer::Batch MyDebugRenderer::CreateTriangleBatch(const Vertex* inVertices, int inVertexCount,
    const JPH::uint32* inIndices, int inIndexCount)
{
    return nullptr;
}

void MyDebugRenderer::DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds,
    float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode,
    ECastShadow inCastShadow, EDrawMode inDrawMode)
{
}

void MyDebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor,
    float inHeight)
{
}

PhysicsEngine::PhysicsEngine(): temp_allocator(nullptr), job_system(nullptr)
{
}

PhysicsEngine::~PhysicsEngine()
{
    delete temp_allocator;
    delete job_system;
}

void PhysicsEngine::Initialize(float gravityScale)
{
    JPH::RegisterDefaultAllocator();
    JPH::Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    temp_allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
    job_system = new JPH::JobSystemThreadPool(
        JPH::cMaxPhysicsJobs,
        JPH::cMaxPhysicsBarriers,
        JPH::thread::hardware_concurrency() - 1
        );

    physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

    physics_system.SetBodyActivationListener(&body_activation_listener);
    physics_system.SetContactListener(&contact_listener);
    physics_system.SetGravity(JPH::Vec3(0.0f, -9.81f / 100 * gravityScale, 0.0f));
}

void PhysicsEngine::UpdatePhysics(float deltaTime)
{
    physics_system.Update(cDeltaTime, cCollisionSteps, temp_allocator, job_system);
}

JPH::PhysicsSystem* PhysicsEngine::GetPhysicsSystem()
{
    return &physics_system;
}

JPH::DebugRenderer* PhysicsEngine::GetDebugRenderer() const
{
    return debug_renderer;
}

void PhysicsEngine::Cleanup()
{
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}
