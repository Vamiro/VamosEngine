#include "PongApp.h"

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
        if (args == InputKey::D3)
        {
            physicsEngine->GetPhysicsSystem()->GetBodyInterface().AddImpulse(
                ball->GetComponent<ColliderComponent>()->GetBody()->GetID(),
                JPH::Vec3Arg(0.0f, 10.0f, 0.0f));
            JPH::Vec3 velocity = physicsEngine->GetPhysicsSystem()->GetBodyInterface().GetLinearVelocity(
                ball->GetComponent<ColliderComponent>()->GetBody()->GetID());
            std::cout << "Velocity: " << velocity.GetX() << " " << velocity.GetY() << " " << velocity.GetZ() << std::endl;

            JPH::RVec3 position = physicsEngine->GetPhysicsSystem()->GetBodyInterface().GetCenterOfMassPosition(
                ball->GetComponent<ColliderComponent>()->GetBody()->GetID());
            std::cout << "Position: " << position.GetX() << " " << position.GetY() << " " << position.GetZ() << std::endl;
        }
    };

    input_device_.OnMouseMove().AddLambda([&](const InputDevice::MouseMoveEventArgs& args) {
         if(input_device_.IsKeyDown(InputKey::RightButton))
             currentCamera->transform->AdjustRotation(
                    -args.Offset.x * input_device_.MouseSensitivity,
                    args.Offset.y * input_device_.MouseSensitivity,
                    0.0f);
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

    if (input_device_.IsKeyDown(InputKey::W))
    {
       currentCamera->transform->AdjustPosition(currentCamera->transform->GetForwardVector() * cameraSpeed * deltaTime);
    }
    if (input_device_.IsKeyDown(InputKey::S))
    {
        currentCamera->transform->AdjustPosition(currentCamera->transform->GetBackwardVector() * cameraSpeed * deltaTime);
    }
    if (input_device_.IsKeyDown(InputKey::A))
    {
        currentCamera->transform->AdjustPosition(currentCamera->transform->GetLeftVector() * cameraSpeed * deltaTime);
    }
    if (input_device_.IsKeyDown(InputKey::D))
    {
        currentCamera->transform->AdjustPosition(currentCamera->transform->GetRightVector() * cameraSpeed * deltaTime);
    }
    if (input_device_.IsKeyDown(InputKey::Q))
    {
        currentCamera->transform->AdjustPosition(currentCamera->transform->GetUpVector() * cameraSpeed * deltaTime);
    }
    if (input_device_.IsKeyDown(InputKey::E))
    {
        currentCamera->transform->AdjustPosition(currentCamera->transform->GetDownVector() * cameraSpeed * deltaTime);
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

    auto gameObject_getter = [](void* data, int index, const char** output) -> bool
    {
        auto* gameObjects = static_cast<std::vector<Object*>*>(data);
        if (index < 0 || index >= static_cast<int>(gameObjects->size()))
            return false;

        Object* current_gameObject = (*gameObjects)[index];
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
        //         currentScene.objects.push_back(*gameObject);
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
        //         gameObjects.push_back(&gameObject);
        //         gameObject.Start();
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

void PongApp::AddComponentToObject(Object* obj, const std::string& component_name)
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

        ball = gameObjects.emplace_back(new GameObject("Obj2"));
        ball->transform->SetPosition(SimpleMath::Vector3(0.0f, 5.0f, 0.0f));
        ball->AddComponent(new Model(*ball, d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader));
        ball->GetComponent<Model>()->SetModelPath("Data\\Objects\\sphere.obj");
        ball->GetComponent<Model>()->SetColor({0.0f, 0.0f, 1.0f, 1.0f});
        auto* c = new ColliderComponent(*ball, physicsEngine->GetPhysicsSystem(), JPH::EMotionType::Dynamic, Layers::MOVING);
        c->SetShape(new JPH::SphereShape(1.0f));
        ball->AddComponent(c);

        floor = gameObjects.emplace_back(new GameObject("Obj2"));
        floor->AddComponent(new Model(*floor, d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader));
        floor->GetComponent<Model>()->SetModelPath("Data\\Objects\\box.obj");
        c = new ColliderComponent(*floor, physicsEngine->GetPhysicsSystem(), JPH::EMotionType::Static, Layers::NON_MOVING);
        c->SetShape(new JPH::BoxShape(JPH::Vec3(10.0f, 1.0f, 10.0f)));
        floor->AddComponent(c);

        auto* camera = new Camera();

        camera->transform->SetPosition(SimpleMath::Vector3(0.0f, 10.0f, -20.0f));
        camera->transform->SetEulerRotate(SimpleMath::Vector3(0.0f, 10.0f, 0.0f));

        camera->SetProjectionValues(90.0f, static_cast<float>(gfx_.GetWindowWidth()) / static_cast<float>(gfx_.GetWindowHeight()), 0.1f,
                                   1000.0f, PERSPECTIVE);

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
