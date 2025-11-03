#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Register window class
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpszClassName = L"TestClass";

    if (!RegisterClassExW(&wcex))
        return 1;

    // Create window
    HWND hWnd = CreateWindowW(
        L"TestClass",
        L"Test Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        600, 400,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd)
        return 1;

    // Create a simple edit control
    HWND hEdit = CreateWindowW(
        L"EDIT",
        L"Test Edit Control",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        50, 50, 200, 25,
        hWnd, nullptr, hInstance, nullptr
    );

    // Create a static label
    HWND hLabel = CreateWindowW(
        L"STATIC",
        L"Test Label:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        50, 20, 100, 25,
        hWnd, nullptr, hInstance, nullptr
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}



