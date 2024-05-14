#include "InputDevice.h"
#include <iostream>
#include "Engine/Core/Engine.h"


using namespace DirectX::SimpleMath;


InputDevice::InputDevice(Engine* engine) : engine_(engine), MouseWheelDelta(0)
{
	keys_ = new std::unordered_set<InputKey>();

	RAWINPUTDEVICE Rid[2];

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = 0; // adds HID mouse and also ignores legacy mouse messages
	Rid[0].hwndTarget = engine_->GetWindow().GetHWND();

	Rid[1].usUsagePage = 0x01;
	Rid[1].usUsage = 0x06;
	Rid[1].dwFlags = 0; // adds HID keyboard and also ignores legacy keyboard messages
	Rid[1].hwndTarget = engine_->GetWindow().GetHWND();

	if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
	{
		auto errorCode = GetLastError();
		std::cout << "ERROR: " << errorCode << std::endl;
	}
}

InputDevice::~InputDevice()
{
	delete keys_;
}

void InputDevice::KeyboardEvent(KeyboardInputEventArgs args)
{
	bool Break = args.Flags & 0x01;

	auto key = static_cast<InputKey>(args.VKey);

	if (args.MakeCode == 42) key = InputKey::LeftShift;
	if (args.MakeCode == 54) key = InputKey::RightShift;
	
	if(Break) {
		if(keys_->count(key))	RemovePressedKey(key);
	} else {
		if (!keys_->count(key))	AddPressedKey(key);
	}
}

void InputDevice::MouseEvent(RawMouseEventArgs args)
{
	if(args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonDown))
		AddPressedKey(InputKey::LeftButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::LeftButtonUp))
		RemovePressedKey(InputKey::LeftButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonDown))
		AddPressedKey(InputKey::RightButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::RightButtonUp))
		RemovePressedKey(InputKey::RightButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonDown))
		AddPressedKey(InputKey::MiddleButton);
	if (args.ButtonFlags & static_cast<int>(MouseButtonFlags::MiddleButtonUp))
		RemovePressedKey(InputKey::MiddleButton);

	POINT p;
	GetCursorPos(&p);
	if(!ScreenToClient(engine_->GetWindow().GetHWND(), &p))
	{
		ErrorLogger::Log(ErrorLogger::GetLastErrorAsString());
	}

	MousePosition	= Vector2(p.x, p.y);
	MouseOffset		= Vector2(args.X, args.Y);
	MouseWheelDelta = args.WheelDelta;

	const MouseMoveEventArgs moveArgs = {MousePosition, MouseOffset, MouseWheelDelta};

	//printf(" Mouse: posX=%04.4f posY:%04.4f offsetX:%04.4f offsetY:%04.4f, wheelDelta=%04d \n",
	//	MousePosition.x,
	//	MousePosition.y,
	//	MouseOffset.x,
	//	MouseOffset.y,
	//	MouseWheelDelta);
	
	onMouseMove_.Broadcast(moveArgs);
}

void InputDevice::AddPressedKey(InputKey key)
{
	//if (!engine_->isActive) {
	//	return;
	//}
	keys_->insert(key);
	onKeyDown_.Broadcast(key);
}

void InputDevice::RemovePressedKey(InputKey key)
{
	keys_->erase(key);
	onKeyUp_.Broadcast(key);
}

bool InputDevice::IsKeyDown(InputKey key)
{
	return keys_->count(key);
}

bool InputDevice::IsMouseMoved()
{
	return MousePosition != Vector2::Zero;
}

