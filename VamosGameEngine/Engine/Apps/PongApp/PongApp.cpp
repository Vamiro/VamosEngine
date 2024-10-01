#include "PongApp.h"

#include "Engine/Components/BallComponent.h"
#include "Engine/Components/FollowCamera.h"
#include "Engine/Components/ColliderComponent.h"

bool PongApp::Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
    GameEngine::Start(hInstance, window_title, window_class, width, height);

    input_device_.OnKeyDown() += [&](const InputKey& args)
    {
        if(args == InputKey::Escape)
        {
            isPaused = !isPaused;
        }
        if(args == InputKey::F4)
        {
            isClosed = true;
        }
        if (args == InputKey::D1)
        {
            ball->GetComponent<Model>()->SetModelPath("Data\\Objects\\box.obj");
        }
        if (args == InputKey::D2)
        {
            ball->GetComponent<Model>()->SetModelPath("Data\\Objects\\sphere.obj");
        }

    };

    input_device_.OnMouseMove().AddLambda([&](const InputDevice::MouseMoveEventArgs& args) {
         if(input_device_.IsKeyDown(InputKey::RightButton))
         {
             auto rot = SimpleMath::Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(-args.Offset.x * input_device_.MouseSensitivity), 0, 0);

             currentCamera->transform->RotateAround(
                 ball->transform->GetPositionVector(),
                 rot);
            //std::cout << std::format("Rotation: ({:.2f}, {:.2f}, {:.2f})", rot.x, rot.y, rot.z) << std::endl;
         }
     });

    return true;
}

void PongApp::Update()
{
    if (this->gfx_.blockInputForImGui || isPaused)
    {
        return;
    }

    if (input_device_.IsKeyDown(InputKey::LeftShift) || input_device_.IsKeyDown(InputKey::RightShift))
    {
        return;
    }

    GameEngine::Update();

    float cameraSpeed = 0.01f;

    float ballSpeed = 1.0f;

    JPH::Vec3 velocity(0.0f, 0.0f, 0.0f);
    auto forward = currentCamera->transform->GetForwardVector();
    forward.Normalize();
    forward *= ballSpeed;

    if (input_device_.IsKeyDown(InputKey::W))
    {
        velocity += JPH::Vec3(forward.x, 0.0f, forward.z);
    }
    if (input_device_.IsKeyDown(InputKey::S))
    {
        velocity -= JPH::Vec3(forward.x, 0.0f, forward.z);
    }

    auto right = currentCamera->transform->GetRightVector();
    right.Normalize();
    right *= ballSpeed / 2.0f;

    if (input_device_.IsKeyDown(InputKey::A))
    {
        velocity += JPH::Vec3(right.x, 0.0f, right.z);
    }
    if (input_device_.IsKeyDown(InputKey::D))
    {
        velocity -= JPH::Vec3(right.x, 0.0f, right.z);
    }

    if (input_device_.IsKeyDown(InputKey::Space))
    {
        velocity += JPH::Vec3(0.0f, 0.001f, 0.0f);
    }

    if(velocity != JPH::Vec3(0.0f, 0.0f, 0.0f))
    {
        //velocity *= 1000.0f;
        ball->GetComponent<ColliderComponent>()->GetBody()->SetLinearVelocity(
        {
            velocity.GetX(),
            ball->GetComponent<ColliderComponent>()->GetBody()->GetLinearVelocity().GetY() + velocity.GetY(),
            velocity.GetZ()
        });
    }


    currentCamera->UpdateViewMatrix();
}

void PongApp::RenderGui()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("GUI");
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
    auto forward_vector = currentCamera->transform->GetForwardVector();
    forward_vector.Normalize();
    ImGui::Text("FPS: (%.2f, %.2f, %.2f)", forward_vector.x, forward_vector.y, forward_vector.z);

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

        // if (ImGui::Button("Save"))
        // {
        //     currentScene.objects.clear();
        //     for (auto* gameObject : gameObjects)
        //     {
        //         currentScene.objects.push_back(gameObject);
        //     }
        //     SaveScene(R"(Engine\Data\Scenes\scene.json)", currentScene);
        // }
        //
        // if (ImGui::Button("Load"))
        // {
        //     LoadScene(R"(Engine\Data\Scenes\scene.json)", currentScene);
        //     gameObjects.clear();
        //     for (auto& gameObject : currentScene.objects)
        //     {
        //         gameObjects.push_back(gameObject);
        //         gameObject->Start();
        //     }
        // }

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
    // Логика для добавления различных компонентов в зависимости от выбранного элемента
    if (component_name == "Model")
    {
        obj->AddComponent(new Model(*obj, gfx_.GetDevice().Get(), gfx_.GetDeviceContext().Get(), cb_vs_vertexshader, cb_ps_pixelshader));
    }


    // Выводим информацию в лог или консоль о добавленном компоненте
    std::cout << "Added component: " << component_name << " to " << obj->name << std::endl;
}

bool PongApp::InitializeScene()
{
    try
    {
        Microsoft::WRL::ComPtr<ID3D11Device> d3d_device = gfx_.GetDevice();
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d_device_context = gfx_.GetDeviceContext();

        HRESULT hr = cb_vs_vertexshader.Initialize(d3d_device.Get(), d3d_device_context.Get());
        ErrorLogger::Log(hr, "Failed to create vertexshader constant buffer.");
        hr = cb_ps_pixelshader.Initialize(d3d_device.Get(), d3d_device_context.Get());
        ErrorLogger::Log(hr, "Failed to create pixelshader constant buffer.");

        ball = gameObjects.emplace_back(new GameObject("Ball"));
        ball->transform->SetPosition(SimpleMath::Vector3(0.0f, 5.0f, 0.0f));
        ball->AddComponent(new Model(*ball, d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader));
        ball->GetComponent<Model>()->SetModelPath("Data\\Objects\\sphere.obj");
        ball->GetComponent<Model>()->SetColor({0.0f, 0.0f, 1.0f, 1.0f});
        auto* c = new ColliderComponent(*ball, physicsEngine->GetPhysicsSystem(), JPH::EMotionType::Dynamic, Layers::MOVING, false);
        c->SetShape(new JPH::SphereShape(1.0f));
        ball->AddComponent(c);
        ball->AddComponent(new BallComponent(*ball));


        for (auto i = -5; i <= 5; ++i)
        {
            for (auto j = -5; j <= 5; ++j)
            {
                auto* boo = gameObjects.emplace_back(new GameObject("Obj" + std::to_string(i) + std::to_string(j)));
                boo->transform->SetPosition(SimpleMath::Vector3(static_cast<float>(i) * 4.0f + 5, 2.0f, static_cast<float>(j) * 4.0f));

                boo->AddComponent(new Model(*boo, d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader));
                boo->GetComponent<Model>()->SetModelPath("Data\\Objects\\sphere.obj");

                // Calculate color based on iterator and grid position
                float r = static_cast<float>(i + 5) / 10.0f;
                float g = static_cast<float>(j + 5) / 10.0f;
                float b = 1.0f - r;
                boo->GetComponent<Model>()->SetColor({r, g, b, 1.0f});

                c = new ColliderComponent(*boo, physicsEngine->GetPhysicsSystem(), JPH::EMotionType::Static, Layers::NON_MOVING, true, true);
                c->SetShape(new JPH::SphereShape(1.0f));
                boo->AddComponent(c);
            }
        }


        floor = gameObjects.emplace_back(new GameObject("Floor"));
        floor->AddComponent(new Model(*floor, d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader));
        floor->GetComponent<Model>()->SetModelPath("Data\\Objects\\box.obj");
        floor->transform->SetScale({100.0f, 1.0f, 100.0f});
        c = new ColliderComponent(*floor, physicsEngine->GetPhysicsSystem(), JPH::EMotionType::Static, Layers::NON_MOVING);
        c->SetShape(new JPH::BoxShape(JPH::Vec3(200.0f, 1.0f, 200.0f)));
        floor->AddComponent(c);

        auto* camera = new Camera();

        camera->transform->SetPosition(SimpleMath::Vector3(0.0f, 10.0f, -10.0f));
        camera->transform->SetEulerRotate(SimpleMath::Vector3(0.0f, 45.0f, 0.0f));

        camera->SetProjectionValues(90.0f, static_cast<float>(gfx_.GetWindowWidth()) / static_cast<float>(gfx_.GetWindowHeight()), 0.1f,
                                   1000.0f, PERSPECTIVE);

        camera->AddComponent(new FollowCamera(*camera, *ball));
        gameObjects.emplace_back(camera);
        SetCurrentCamera(camera);
    }
    catch (COMException& exception)
    {
        ErrorLogger::Log(exception);
        return false;
    }
    return true;
}
