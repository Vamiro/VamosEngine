#include "Engine.h"

InputDevice* Engine::input_device_ = nullptr;
Camera* Engine::currentCamera = nullptr;
Graphics* Engine::gfx_ = nullptr;

Engine::Engine()
{
    input_device_ = new InputDevice(this);
    gfx_ = new Graphics(this);
    currentCamera = new Camera();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg) {
    case WM_DESTROY:
    case WM_QUIT: {
        isClosed = true;
        return 0;
    }
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            // The window was minimized (you should probably suspend the application)
            if (!s_minimized)
            {
                s_minimized = true;
            }
        }
        else if (s_minimized)
        {
            // The window was minimized and is now restored (resume from suspend)
            s_minimized = false;
        }
        else if ( !s_in_sizemove )
        {
            RECT rect;
            if (GetClientRect(hWnd, &rect))
            {
                gfx_->OnResize(rect.right - rect.left, rect.bottom - rect.top);
            }
        }
        break;

    case WM_ENTERSIZEMOVE:
        // We want to avoid trying to resizing the swapchain as the user does the 'rubber band' resize
        s_in_sizemove = true;
        break;

    case WM_EXITSIZEMOVE:
        s_in_sizemove = false;
        // Here is the other place where you handle the swapchain resize after the user stops using the 'rubber-band'
        RECT rect;
        if (GetClientRect(hWnd, &rect))
        {
            gfx_->OnResize(rect.right - rect.left, rect.bottom - rect.top);
        }
        break;

    case WM_GETMINMAXINFO:
        {
            // We want to prevent the window from being set too tiny
            auto info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = 320;
            info->ptMinTrackSize.y = 200;
        }
        break;
    case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE) {
                isFocused = false;
            } else {
                isFocused = true;
            }
            return 0;
    }
    case WM_INPUT: {
        UINT dwSize = 0;
        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
        LPBYTE lpb = new BYTE[dwSize];

        if (lpb == nullptr) {
            return 0;
        }

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
            ErrorLogger::Log("GetRawInputData does not return correct size !");

        RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

        if (raw->header.dwType == RIM_TYPEKEYBOARD)
            /* fprintf(stderr, " Kbd: make=%04i Flags:%04i Reserved:%04i ExtraInformation:%08i, msg=%04i VK=%i \n",
                 raw->data.keyboard.MakeCode,
                 raw->data.keyboard.Flags,
                 raw->data.keyboard.Reserved,
                 raw->data.keyboard.ExtraInformation,
                 raw->data.keyboard.Message,
                 raw->data.keyboard.VKey);
                 */
            input_device_->KeyboardEvent({
                raw->data.keyboard.MakeCode,
                raw->data.keyboard.Flags,
                raw->data.keyboard.VKey,
                raw->data.keyboard.Message
            });
        else if (raw->header.dwType == RIM_TYPEMOUSE)
        {
            //printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
            input_device_->MouseEvent({
                raw->data.mouse.usFlags,
                raw->data.mouse.usButtonFlags,
                static_cast<int>(raw->data.mouse.ulExtraInformation),
                static_cast<int>(raw->data.mouse.ulRawButtons),
                static_cast<short>(raw->data.mouse.usButtonData),
                static_cast<int>(raw->data.mouse.lLastX),
                static_cast<int>(raw->data.mouse.lLastY)
            });
        }

        delete[] lpb;
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    default: {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    }
    return 0;
}
