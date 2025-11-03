#include "pch.h"
#include "RegExpCalc.h"
#include "Resource.h"
#include "RegExpEngine.h"
#include <algorithm>

// ===== Константы размещения =====
const int BORDER_MARGIN = 8;
const int LABEL_WIDTH = 90;
const int CONTROL_HEIGHT = 25;


// ====== Утилита для подсчета строк в результате ======
int CountLines(const std::string& s) {
    return std::count(s.begin(), s.end(), '\n') + 1;
}

// ========== Класс ===============

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static RegExpCalc* pCalc = nullptr;

    switch (uMsg)
    {
    case WM_CREATE:
        pCalc = new RegExpCalc();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCalc);
        pCalc->Initialize(hwnd);
        return 0;

    case WM_COMMAND:
    {
        if (pCalc)
        {
            WORD notificationCode = HIWORD(wParam);
            HWND hCtrl = (HWND)lParam;

            if (notificationCode == CBN_SELCHANGE) {
                int selection = (int)SendMessage(hCtrl, CB_GETCURSEL, 0, 0);
                if (selection != CB_ERR) {
                    int ctrlId = GetDlgCtrlID(hCtrl);
                    pCalc->LoadHistoryItem(selection, pCalc->GetCurrentMode(), ctrlId);
                }
                return 0;
            }
            if (notificationCode == CBN_EDITCHANGE) {
                pCalc->OnTextChanged();
                return 0;
            }
            pCalc->OnCommand(LOWORD(wParam));
        }
        break;
    }
    case WM_KEYDOWN:
        if (pCalc)
            pCalc->OnKeyDown(wParam);
        break;

    case WM_SIZE:
        if (pCalc)
            pCalc->UpdateLayout();
        break;

    case WM_MOVE:
        if (pCalc) {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            pCalc->OnMove(rect.left, rect.top);
        }
        break;

    case WM_PAINT:
        if (pCalc)
            pCalc->OnPaint();
        break;

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
        if (pCalc && lParam) {
            WORD ctrlId = GetDlgCtrlID((HWND)lParam);
            if (ctrlId != 0) {
                SetTextColor((HDC)wParam, RGB(0, 0, 0));
                SetBkColor((HDC)wParam, RGB(255, 255, 255));
                return (LRESULT)GetStockObject(WHITE_BRUSH);
            }
        }
        break;

    case WM_DESTROY:
        if (pCalc) pCalc->OnDestroy();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// ----

RegExpCalc::RegExpCalc()
    : m_hInst(nullptr),
      m_hWnd(nullptr),
      m_hSourceInput(nullptr),
      m_hRegExpInput(nullptr),
      m_hReplaceInput(nullptr),
      m_hFormatInput(nullptr),
      m_hParametersInput(nullptr),
      m_hDataInput(nullptr),
      m_hResultEdit(nullptr),
      m_hSourceLabel(nullptr),
      m_hRegExpLabel(nullptr),
      m_hReplaceLabel(nullptr),
      m_hFormatLabel(nullptr),
      m_hParametersLabel(nullptr),
      m_hDataLabel(nullptr),
      m_hMenu(nullptr),
      m_currentMode(AppMode::REGEXP),
      m_currentEngine(RegExpEngineType::CLASSIC),
      m_fontSize(DEFAULT_FONT_SIZE),
      m_opacity(100), m_alwaysOnTop(false),
      m_uiReady(false),
      m_windowX(100), m_windowY(100),
      m_windowWidth(700), m_windowHeight(300),
      m_dpiX(96), m_dpiY(96),
      m_hWhiteBrush(nullptr),
      m_hFont(nullptr),
      m_originalInputProc(nullptr),
      m_originalComboProc(nullptr)
{
    m_hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    CreateRegExpEngine(m_currentEngine);
}

RegExpCalc::~RegExpCalc() {}

BOOL RegExpCalc::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    m_hInst = hInstance;
    if (!RegisterClass(hInstance)) return FALSE;
    LoadSettings();
    m_hWnd = CreateWindow(L"RegExpCalcClass", L"RegExp Calculator",
        WS_OVERLAPPEDWINDOW,
        m_windowX, m_windowY,
        m_windowWidth, m_windowHeight,
        nullptr, nullptr, hInstance, nullptr);
    if (!m_hWnd) return FALSE;
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
    return TRUE;
}

ATOM RegExpCalc::RegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REGEXPCALC));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_REGEXPCALC);
    wcex.lpszClassName = L"RegExpCalcClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

void RegExpCalc::Initialize(HWND hwnd)
{
    m_hWnd = hwnd;
    InitializeDPI();
    UpdateFont();
    OnCreate();
    LoadHistory();
    LoadSettings();
    m_uiReady = true;
    UpdateLayoutForMode(m_currentMode);
    EvaluateExpression();
}

void RegExpCalc::OnCreate()
{
    m_hSourceLabel = CreateWindowW(L"STATIC", L"Source:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_SOURCE_LABEL, m_hInst, nullptr);
    m_hRegExpLabel = CreateWindowW(L"STATIC", L"RegExp:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_REGEXP_LABEL, m_hInst, nullptr);
    m_hReplaceLabel = CreateWindowW(L"STATIC", L"Replace:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_REPLACE_LABEL, m_hInst, nullptr);
    m_hFormatLabel = CreateWindowW(L"STATIC", L"Format:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_FORMAT_LABEL, m_hInst, nullptr);
    m_hParametersLabel = CreateWindowW(L"STATIC", L"Parameters:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_PARAMETERS_LABEL, m_hInst, nullptr);
    m_hDataLabel = CreateWindowW(L"STATIC", L"Data:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_DATA_LABEL, m_hInst, nullptr);

    m_hSourceInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_SOURCE_EDIT, m_hInst, nullptr);

    m_hRegExpInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_REGEXP_EDIT, m_hInst, nullptr);

    m_hReplaceInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_REPLACE_EDIT, m_hInst, nullptr);

    m_hFormatInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_FORMAT_EDIT, m_hInst, nullptr);

    m_hParametersInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_PARAMETERS_EDIT, m_hInst, nullptr);

    m_hDataInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_DATA_EDIT, m_hInst, nullptr);

    m_hResultEdit = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_READONLY,
        0, 0, 10, 10, m_hWnd, (HMENU)IDC_RESULT_EDIT, m_hInst, nullptr);

    // Шрифты
    SendMessage(m_hSourceLabel, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hRegExpLabel, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hReplaceLabel, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hFormatLabel, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hParametersLabel, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hDataLabel, WM_SETFONT, (WPARAM)m_hFont, TRUE);

    SendMessage(m_hSourceInput, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hRegExpInput, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hReplaceInput, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hFormatInput, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hParametersInput, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hDataInput, WM_SETFONT, (WPARAM)m_hFont, TRUE);
    SendMessage(m_hResultEdit, WM_SETFONT, (WPARAM)m_hFont, TRUE);

    m_originalComboProc = (WNDPROC)SetWindowLongPtr(m_hSourceInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hRegExpInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hReplaceInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hFormatInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hParametersInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hDataInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
}

void RegExpCalc::UpdateLayout()
{
    UpdateLayoutForMode(m_currentMode);
}

void RegExpCalc::UpdateLayoutForMode(AppMode mode)
{
    RECT rect;
    GetClientRect(m_hWnd, &rect);

    int width = rect.right - rect.left;
    int x = BORDER_MARGIN;
    int y = BORDER_MARGIN;
    int cbWidth = width - LABEL_WIDTH - 2 * BORDER_MARGIN;

    ShowControlsForMode(mode);

    if (mode == AppMode::REGEXP)
    {
        MoveWindow(m_hSourceLabel, x, y, LABEL_WIDTH, CONTROL_HEIGHT, TRUE);
        MoveWindow(m_hSourceInput, x + LABEL_WIDTH, y, cbWidth, CONTROL_HEIGHT, TRUE);
        y += CONTROL_HEIGHT + BORDER_MARGIN;

        MoveWindow(m_hRegExpLabel, x, y, LABEL_WIDTH, CONTROL_HEIGHT, TRUE);
        MoveWindow(m_hRegExpInput, x + LABEL_WIDTH, y, cbWidth, CONTROL_HEIGHT, TRUE);
        y += CONTROL_HEIGHT + BORDER_MARGIN;

        MoveWindow(m_hReplaceLabel, x, y, LABEL_WIDTH, CONTROL_HEIGHT, TRUE);
        MoveWindow(m_hReplaceInput, x + LABEL_WIDTH, y, cbWidth, CONTROL_HEIGHT, TRUE);
        y += CONTROL_HEIGHT + BORDER_MARGIN;
    }
    else if (mode == AppMode::PRINTF)
    {
        MoveWindow(m_hFormatLabel, x, y, LABEL_WIDTH, CONTROL_HEIGHT, TRUE);
        MoveWindow(m_hFormatInput, x + LABEL_WIDTH, y, cbWidth, CONTROL_HEIGHT, TRUE);
        y += CONTROL_HEIGHT + BORDER_MARGIN;

        MoveWindow(m_hParametersLabel, x, y, LABEL_WIDTH, CONTROL_HEIGHT, TRUE);
        MoveWindow(m_hParametersInput, x + LABEL_WIDTH, y, cbWidth, CONTROL_HEIGHT, TRUE);
        y += CONTROL_HEIGHT + BORDER_MARGIN;
    }
    else if (mode == AppMode::SCANF)
    {
        MoveWindow(m_hFormatLabel, x, y, LABEL_WIDTH, CONTROL_HEIGHT, TRUE);
        MoveWindow(m_hFormatInput, x + LABEL_WIDTH, y, cbWidth, CONTROL_HEIGHT, TRUE);
        y += CONTROL_HEIGHT + BORDER_MARGIN;

        MoveWindow(m_hDataLabel, x, y, LABEL_WIDTH, CONTROL_HEIGHT, TRUE);
        MoveWindow(m_hDataInput, x + LABEL_WIDTH, y, cbWidth, CONTROL_HEIGHT, TRUE);
        y += CONTROL_HEIGHT + BORDER_MARGIN;
    }

    // Получаем реальное число строк вывода
    int resultLines = 1;
    {
        int len = GetWindowTextLengthA(m_hResultEdit);
        std::string text(len, '\0');
        if (len > 0) GetWindowTextA(m_hResultEdit, &text[0], len + 1);
        resultLines = (len > 0) ? CountLines(text) : 1;
        resultLines = std::min(std::max(resultLines, 1), 20);
    }
    int resultHeight = resultLines * CONTROL_HEIGHT + BORDER_MARGIN;
    MoveWindow(m_hResultEdit, x, y, width - 2 * BORDER_MARGIN, resultHeight, TRUE);

    // Итоговая требуемая высота окна:
    int totalHeight = y + resultHeight + BORDER_MARGIN;
    int minHeight = 4 * CONTROL_HEIGHT + 6 * BORDER_MARGIN; // минимум: меню + 3 строки + вывод
    int maxHeight = 20 * CONTROL_HEIGHT + 6 * BORDER_MARGIN; // максимум

    totalHeight = std::min(std::max(totalHeight, minHeight), maxHeight);
    RECT win;
    GetWindowRect(m_hWnd, &win);
    MoveWindow(m_hWnd, win.left, win.top, width, totalHeight, TRUE);
}

// ---- Остальной код (без изменений) ----
// (сюда копируйте всё, что не связано с геометрией управления, из вашей рабочей версии: всё что выше про обработку текста, меню и т. д.)

// ... вставьте остальной код класса RegExpCalc из своего файла сюда ...