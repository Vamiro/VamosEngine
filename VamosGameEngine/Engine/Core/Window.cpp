﻿#include "Window.h"
#include "Engine.h"

//TODO: Add support for fullscreen mode, write comments

bool Window::Initialize(Engine* pEngine, HINSTANCE hInstance, std::string window_title,
                        std::string window_class, int width, int height)
{
    this->hInstance = hInstance;
    this->width = width;
    this->height = height;
    this->window_title = window_title;
    this->window_title_wide = StringHelper::StringToWide(this->window_title);
    this->window_class = window_class;
    this->window_class_wide = StringHelper::StringToWide(this->window_class);
    //wide string representation of class string (used for registering class and creating window)

    this->RegisterWindowClass();
    int screenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2 - this->width / 2;
    int screenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2 - this->height / 2;


    RECT wr;
    wr.left = screenCenterX;
    wr.right = wr.left + this->width;
    wr.top = screenCenterY;
    wr.bottom = wr.top + this->height;
    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

    this->handle = CreateWindowEx(
        0,
        //Extended Windows style - we are using the default. For other options, see: https://msdn.microsoft.com/en-us/library/windows/desktop/ff700543(v=vs.85).aspx
        this->window_class_wide.c_str(), //Window class name
        this->window_title_wide.c_str(), //Window Title
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        //Windows style - See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632600(v=vs.85).aspx
        wr.left, //Window X Position
        wr.top, //Window Y Position
        wr.right - wr.left, //Window Width
        wr.bottom - wr.top, //Window Height
        NULL, //Handle to parent of this window. Since this is the first window, it has no parent window.
        NULL,
        //Handle to menu or child window identifier. Can be set to NULL and use menu in WindowClassEx if a menu is desired to be used.
        this->hInstance, //Handle to the instance of module to be used with this window
        pEngine); //Param to create window

    if (this->handle == NULL)
    {
        ErrorLogger::Log(GetLastError(), "CreateWindowEX Failed for window: " + this->window_title);
        return false;
    }

    // Bring the window up on the screen and set it as main focus.
    ShowWindow(this->handle, SW_SHOW);
    SetForegroundWindow(this->handle);
    SetFocus(this->handle);

    return true;
}

Window::~Window()
{
    if (this->handle != NULL)
    {
        UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
        DestroyWindow(handle);
    }
}

bool Window::ProcessMessages()
{
    // Handle the windows messages.
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG)); // Initialize the message structure.

    while (PeekMessage(&msg, this->handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Check if the window was closed
    if (msg.message == WM_NULL)
    {
        if (!IsWindow(this->handle))
        {
            this->handle = NULL; //Message processing loop takes care of destroying this window
            UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
            return false;
        }
    }

    return true;
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    // All other messages
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    default:
        {
            // retrieve ptr to window class
            Engine* const pEngine = reinterpret_cast<Engine*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            // forward message to window class handler
            return pEngine->WindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM LParam)
{
    switch (uMsg)
    {
    case WM_NCCREATE:
        {
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(LParam);
            Engine* pEngine = reinterpret_cast<Engine*>(pCreate->lpCreateParams);
            if (pEngine == nullptr)
            {
                ErrorLogger::Log("Critical Error: Pointer to Window Container is null during WM_NCREATE\n");
                exit(-1);
            }
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pEngine));
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
            return pEngine->WindowProc(hwnd, uMsg, wParam, LParam);
        }
    default:
        {
            return DefWindowProc(hwnd, uMsg, wParam, LParam);
        }
    }
}

void Window::RegisterWindowClass()
{
    WNDCLASSEX wc;
    //Our Window Class (This has to be filled before our window can be created) See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633577(v=vs.85).aspx
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    //Flags [Redraw on width/height change from resize/movement] See: https://msdn.microsoft.com/en-us/library/windows/desktop/ff729176(v=vs.85).aspx
    wc.lpfnWndProc = HandleMessageSetup; //Pointer to Window Proc function for handling messages from this window
    wc.cbClsExtra = 0;
    //# of extra bytes to allocate following the window-class structure. We are not currently using this.
    wc.cbWndExtra = 0; //# of extra bytes to allocate following the window instance. We are not currently using this.
    wc.hInstance = this->hInstance; //Handle to the instance that contains the Window Procedure
    wc.hIcon = NULL;
    //Handle to the class icon. Must be a handle to an icon resource. We are not currently assigning an icon, so this is null.
    wc.hIconSm = NULL; //Handle to small icon for this class. We are not currently assigning an icon, so this is null.
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //Default Cursor - If we leave this null, we have to explicitly set the cursor's shape each time it enters the window.
    wc.hbrBackground = NULL;
    //Handle to the class background brush for the window's background color - we will leave this blank for now and later set this to black. For stock brushes, see: https://msdn.microsoft.com/en-us/library/windows/desktop/dd144925(v=vs.85).aspx
    wc.lpszMenuName = NULL;
    //Pointer to a null terminated character string for the menu. We are not using a menu yet, so this will be NULL.
    wc.lpszClassName = this->window_class_wide.c_str();
    //Pointer to null terminated string of our class name for this window.
    wc.cbSize = sizeof(WNDCLASSEX); //Need to fill in the size of our struct for cbSize
    RegisterClassEx(&wc); // Register the class so that it is usable.
}
