cmake_minimum_required(VERSION 3.28)
project(VamosGameEngine)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(UNICODE)

set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/VamosGameEngine/includes)
set(DATA_DIRECTORY ${CMAKE_SOURCE_DIR}/VamosGameEngine/Engine/Data)

include_directories(VamosGameEngine)
include_directories(VamosGameEngine/Engine/Core/Input)
include_directories(VamosGameEngine/Engine/Utilities/Delegate)
include_directories(VamosGameEngine/includes)
include_directories(VamosGameEngine/ThirdParty)

add_executable(VamosGameEngine WIN32
        VamosGameEngine/Engine/Core/Input/InputDevice.cpp
        VamosGameEngine/Engine/Core/Input/InputDevice.h
        VamosGameEngine/Engine/Core/Input/Key.h
        VamosGameEngine/Engine/Core/Engine.cpp
        VamosGameEngine/Engine/Core/Engine.h
        VamosGameEngine/Engine/Core/GameEngine.cpp
        VamosGameEngine/Engine/Core/GameEngine.h
        VamosGameEngine/Engine/Core/Window.cpp
        VamosGameEngine/Engine/Core/Window.h
        VamosGameEngine/Engine/Graphics/Adapters.cpp
        VamosGameEngine/Engine/Graphics/Adapters.h
        VamosGameEngine/Engine/Graphics/ConstantBuffer.h
        VamosGameEngine/Engine/Graphics/Graphics.cpp
        VamosGameEngine/Engine/Graphics/Graphics.h
        VamosGameEngine/Engine/Graphics/IndexBuffer.h
        VamosGameEngine/Engine/Graphics/Shaders.cpp
        VamosGameEngine/Engine/Graphics/Shaders.h
        VamosGameEngine/Engine/Graphics/Vertex.h
        VamosGameEngine/Engine/Graphics/VertexBuffer.h
        VamosGameEngine/Engine/Rendering/Camera.cpp
        VamosGameEngine/Engine/Rendering/Camera.h
        VamosGameEngine/Engine/Core/Object.cpp
        VamosGameEngine/Engine/Core/Object.h
        VamosGameEngine/Engine/Rendering/Mesh.cpp
        VamosGameEngine/Engine/Rendering/Mesh.h
        VamosGameEngine/Engine/Components/Model.cpp
        VamosGameEngine/Engine/Components/Model.h
        VamosGameEngine/Engine/Rendering/Texture.cpp
        VamosGameEngine/Engine/Rendering/Texture.h
        VamosGameEngine/Engine/Utilities/Delegate/Delegate.cpp
        VamosGameEngine/Engine/Utilities/Delegate/Delegate.h
        VamosGameEngine/Engine/Utilities/COMException.h
        VamosGameEngine/Engine/Utilities/ConcurrentQueue.h
        VamosGameEngine/Engine/Utilities/ErrorLogger.cpp
        VamosGameEngine/Engine/Utilities/ErrorLogger.h
        VamosGameEngine/Engine/Utilities/Singleton.h
        VamosGameEngine/Engine/Utilities/StringHelper.cpp
        VamosGameEngine/Engine/Utilities/StringHelper.h
        VamosGameEngine/Engine/Utilities/Timer.cpp
        VamosGameEngine/Engine/Utilities/Timer.h
        VamosGameEngine/ThirdParty/ImGUI/imconfig.h
        VamosGameEngine/ThirdParty/ImGUI/imgui.cpp
        VamosGameEngine/ThirdParty/ImGUI/imgui.h
        VamosGameEngine/ThirdParty/ImGUI/imgui_draw.cpp
        VamosGameEngine/ThirdParty/ImGUI/imgui_impl_dx11.cpp
        VamosGameEngine/ThirdParty/ImGUI/imgui_impl_dx11.h
        VamosGameEngine/ThirdParty/ImGUI/imgui_impl_win32.cpp
        VamosGameEngine/ThirdParty/ImGUI/imgui_impl_win32.h
        VamosGameEngine/ThirdParty/ImGUI/imgui_internal.h
        VamosGameEngine/ThirdParty/ImGUI/imgui_tables.cpp
        VamosGameEngine/ThirdParty/ImGUI/imgui_widgets.cpp
        VamosGameEngine/ThirdParty/ImGUI/imstb_rectpack.h
        VamosGameEngine/ThirdParty/ImGUI/imstb_textedit.h
        VamosGameEngine/ThirdParty/ImGUI/imstb_truetype.h
        VamosGameEngine/VamosGameEngine.cpp
        VamosGameEngine/Engine/Components/Transform.cpp
        VamosGameEngine/Engine/Components/Transform.h
        VamosGameEngine/Engine/Graphics/ShaderManager.cpp
        VamosGameEngine/Engine/Graphics/ShaderManager.h
        VamosGameEngine/Engine/Apps/PongApp/GameObjects/SphereObject.cpp
        VamosGameEngine/Engine/Apps/PongApp/GameObjects/SphereObject.h
        VamosGameEngine/Engine/Apps/TestApp/TestApp.cpp
        VamosGameEngine/Engine/Apps/TestApp/TestApp.h
        VamosGameEngine/Engine/Apps/PongApp/PongApp.cpp
        VamosGameEngine/Engine/Apps/PongApp/PongApp.h
        VamosGameEngine/Engine/Apps/PongApp/GameObjects/BoxObject.cpp
        VamosGameEngine/Engine/Apps/PongApp/GameObjects/BoxObject.h
        VamosGameEngine/Engine/Components/Component.h
        VamosGameEngine/Engine/Core/GameObject.cpp
        VamosGameEngine/Engine/Core/GameObject.h
        VamosGameEngine/Engine/Components/ModelBuffer.h
        VamosGameEngine/Engine/Physics/PhysicsEngine.cpp
        VamosGameEngine/Engine/Physics/PhysicsEngine.h
        VamosGameEngine/Engine/Components/ColliderComponent.cpp
        VamosGameEngine/Engine/Components/ColliderComponent.h
        VamosGameEngine/Engine/Utilities/JsonUtil.h
        VamosGameEngine/Engine/Core/Scene.h
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(VamosGameEngine/ThirdParty/JoltPhysics/Build)
add_subdirectory(VamosGameEngine/ThirdParty/DirectXTK ${CMAKE_BINARY_DIR}/bin/CMake/DirectXTK)
add_subdirectory(VamosGameEngine/ThirdParty/assimp)

target_link_libraries(VamosGameEngine d3d11.lib dxgi.lib d3dcompiler.lib DirectXTK assimp Jolt)

add_custom_command(TARGET VamosGameEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${DATA_DIRECTORY}"
        $<TARGET_FILE_DIR:VamosGameEngine>/Data)
