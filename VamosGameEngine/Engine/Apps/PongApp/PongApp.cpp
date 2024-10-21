#include "PongApp.h"

#include "Engine/Components/BallComponent.h"
#include "Engine/Components/FollowCamera.h"
#include "Engine/Components/ColliderComponent.h"
#include "Engine/Components/Model.h"

bool PongApp::Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
    GameEngine::Start(hInstance, window_title, window_class, width, height);

    input_device_->OnKeyDown() += [&](const InputKey& args)
    {
        if(args == InputKey::Escape)
        {
            isPaused = !isPaused;
        }
        if(args == InputKey::F4)
        {
            isClosed = true;
        }
    };
    return true;
}

void PongApp::Update()
{
    GameEngine::Update();

    currentCamera->UpdateViewMatrix();
}

void PongApp::RenderGui()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("GUI");
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

    float pos[3] = {_lightDirection.x, _lightDirection.y, _lightDirection.z};
    ImGui::DragFloat3("", pos, 0.01f, -1.0f, 1.0f);
    _lightDirection = DirectX::SimpleMath::Vector3(pos[0], pos[1], pos[2]);

    auto gameObject_getter = [](void* data, int index, const char** output) -> bool
    {
        auto* gameObjects = static_cast<std::vector<GameObject*>*>(data);
        if (index < 0 || index >= static_cast<int>(gameObjects->size()))
            return false;

        GameObject* current_gameObject = (*gameObjects)[index];
        if (!current_gameObject)
            return false;

        *output = current_gameObject->name.c_str();
        return true;
    };

    ImGui::ListBox(
        "Objects",
        &currentGameObj,
        gameObject_getter,
        &gameObjects,
        gameObjects.size());

    if(ImGui::Button("+"))
    {
        gameObjects.emplace_back(new GameObject("GameObject" + std::to_string(gameObjects.size())));
    }

    gameObjects[currentGameObj]->RenderComponentsGUI();



    if(ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("Add Component");
    }

    if (ImGui::BeginPopupModal("Add Component"))
    {
        const char* components[] = { "Model" };
        static int selected_component = -1;

        ImGui::Text("Select a component to add:");
        if (ImGui::ListBox("##components", &selected_component, components, IM_ARRAYSIZE(components)))
        {
            if (selected_component >= 0 && selected_component < IM_ARRAYSIZE(components))
            {
                AddComponentToObject(gameObjects[currentGameObj], components[selected_component]);
                selected_component = -1;
            }
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }


    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void PongApp::AddComponentToObject(GameObject* obj, const std::string& component_name)
{
    if (component_name == "Model")
    {
        obj->AddComponent(new Model(*obj, gfx_->GetDevice().Get(), gfx_->GetDeviceContext().Get(), cb_vs_vertexshader, cb_ps_pixelshader));
    }
}

bool PongApp::InitializeScene()
{
    try
    {
        Microsoft::WRL::ComPtr<ID3D11Device> d3d_device = gfx_->GetDevice();
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d_device_context = gfx_->GetDeviceContext();

        HRESULT hr = cb_vs_vertexshader.Initialize(d3d_device.Get(), d3d_device_context.Get());
        ErrorLogger::Log(hr, "Failed to create vertexshader constant buffer.");
        hr = cb_ps_pixelshader.Initialize(d3d_device.Get(), d3d_device_context.Get());
        ErrorLogger::Log(hr, "Failed to create pixelshader constant buffer.");

        auto* camera = new Camera();
        camera->transform->SetGlobalPosition(DirectX::SimpleMath::Vector3(0.0f, 10.0f, -10.0f));
        camera->transform->SetEulerRotate(DirectX::SimpleMath::Vector3(0.0f, 45.0f, 0.0f));
        camera->SetProjectionValues(90.0f, static_cast<float>(gfx_->GetWindowWidth()) / static_cast<float>(gfx_->GetWindowHeight()), 0.1f,
                                   1000.0f, PERSPECTIVE);
        gameObjects.emplace_back(camera);
        SetCurrentCamera(camera);

        ball = gameObjects.emplace_back(new GameObject("Ball"));
        ball->transform->SetGlobalPosition(DirectX::SimpleMath::Vector3(0.0f, 10.0f, -30.0f));
        ball->AddComponent(new Model(*ball, d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader));
        ball->GetComponent<Model>()->SetModelPath("Data\\Objects\\sphere.obj");
        ball->GetComponent<Model>()->SetColor({0.0f, 0.0f, 1.0f, 1.0f});
        auto* c = new ColliderComponent(*ball, *_bodyInterface, JPH::EMotionType::Dynamic, Layers::PLAYER);
        c->SetShape(new JPH::SphereShape(1.0f));
        ball->AddComponent(c);
        ball->AddComponent(new FollowCamera(*ball, *camera->transform));
        ball->AddComponent(new BallComponent(*ball));

        for (auto i = -5; i <= 5; ++i)
        {
            for (auto j = -5; j <= 5; ++j)
            {
                auto* boo = gameObjects.emplace_back(new GameObject("Obj" + std::to_string(i) + std::to_string(j)));
                boo->transform->SetGlobalPosition(DirectX::SimpleMath::Vector3(static_cast<float>(i) * 4.0f + 5, 2.0f, static_cast<float>(j) * 4.0f));

                boo->AddComponent(new Model(*boo, d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader));
                boo->GetComponent<Model>()->SetModelPath("Data\\Objects\\sphere.obj");

                float r = static_cast<float>(i + 5) / 10.0f;
                float g = static_cast<float>(j + 5) / 10.0f;
                float b = 1.0f - r;
                boo->GetComponent<Model>()->SetColor({r, g, b, 1.0f});

                c = new ColliderComponent(*boo, *_bodyInterface, JPH::EMotionType::Static, Layers::NON_MOVING, true, true);
                c->SetShape(new JPH::SphereShape(1.0f));
                boo->AddComponent(c);
            }
        }

        floor = gameObjects.emplace_back(new GameObject("Floor"));
        floor->AddComponent(new Model(*floor, d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader));
        floor->GetComponent<Model>()->SetModelPath("Data\\Objects\\box.obj");
        floor->transform->SetGlobalScale({100.0f, 1.0f, 100.0f});
        c = new ColliderComponent(*floor, *_bodyInterface, JPH::EMotionType::Static, Layers::NON_MOVING);
        c->SetShape(new JPH::BoxShape(JPH::Vec3(200.0f, 1.0f, 200.0f)));
        floor->AddComponent(c);
    }
    catch (COMException& exception)
    {
        ErrorLogger::Log(exception);
        return false;
    }
    return true;
}
