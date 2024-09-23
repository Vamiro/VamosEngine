#include "Engine.h"

Engine::Engine(): input_device_(this), gfx_(this)
{
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    switch (uMsg) {
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
                    input_device_.KeyboardEvent({
                        raw->data.keyboard.MakeCode,
                        raw->data.keyboard.Flags,
                        raw->data.keyboard.VKey,
                        raw->data.keyboard.Message
                    });
                else if (raw->header.dwType == RIM_TYPEMOUSE)
                {
                    //printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
                    input_device_.MouseEvent({
                        raw->data.mouse.usFlags,
                        raw->data.mouse.usButtonFlags,
                        static_cast<int>(raw->data.mouse.ulExtraInformation),
                        static_cast<int>(raw->data.mouse.ulRawButtons),
                        static_cast<short>(raw->data.mouse.usButtonData),
                        raw->data.mouse.lLastX,
                        raw->data.mouse.lLastY
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
