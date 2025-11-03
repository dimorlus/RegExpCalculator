#include "pch.h"
#include "RegExpCalc.h"
#include "Resource.h"

// Application entry point
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    RegExpCalc calc;
    if (!calc.InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REGEXPCALC));

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        // Try accelerators first - they have priority
        // Note: TranslateAccelerator sends WM_COMMAND synchronously via SendMessage
        // so if it returns TRUE, the command has already been processed
        if (TranslateAccelerator(calc.GetMainWindow(), hAccelTable, &msg))
        {
            // Accelerator processed, skip further handling of this message
            continue;
        }
        
        // For keyboard messages with Ctrl/Shift/Alt, don't use IsDialogMessage
        // to avoid conflicts with accelerators
        BOOL useDialog = TRUE;
        if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP || msg.message == WM_SYSKEYDOWN || msg.message == WM_SYSKEYUP)
        {
            if ((GetKeyState(VK_CONTROL) & 0x8000) || (GetKeyState(VK_MENU) & 0x8000))
            {
                useDialog = FALSE;
            }
        }
        
        if (useDialog && IsDialogMessage(calc.GetMainWindow(), &msg))
        {
            // Dialog message processed
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}