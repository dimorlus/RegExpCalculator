#pragma once

#include "Resource.h"
#include "pch.h"
#include "RegExpClass.h"
#include "RegExpEngine.h"

// Forward declarations

// Constants  
const int WINDOW_MIN_WIDTH = 600;
const int WINDOW_MIN_HEIGHT = 300;
const int CONTROL_HEIGHT = 25;
const int CONTROL_MARGIN = 5;
const int BORDER_MARGIN = 8;      // <-- добавьте сюда!
const int LABEL_WIDTH = 90;       // <-- добавьте сюда!
const int HISTORY_HEIGHT = 100;
const int MAX_HISTORY = 100;
const int DEFAULT_FONT_SIZE = 14;
const int MIN_FONT_SIZE = 8;
const int MAX_FONT_SIZE = 24;

// Режимы работы приложения
enum class AppMode {
    REGEXP = 0,
    PRINTF = 1,
    SCANF = 2
};

// Типы regexp движков
enum class RegExpEngineType {
    CLASSIC = 0,
    MODERN = 1,
    PYTHON = 2,
    RE2 = 3
};

// Main application class
class RegExpCalc
{
private:
    HINSTANCE m_hInst;
    HWND m_hWnd;
    
    // UI Controls
    HWND m_hSourceInput;      // Исходная строка (RegExp режим)
    HWND m_hRegExpInput;      // Регулярное выражение (RegExp режим)
    HWND m_hReplaceInput;     // Выражение замены (RegExp режим)
    HWND m_hFormatInput;      // Формат (Printf/Scanf режимы)
    HWND m_hParametersInput;  // Параметры (Printf режим)
    HWND m_hDataInput;        // Данные (Scanf режим)
    HWND m_hResultEdit;      // Поле результата
    
    // Labels
    HWND m_hSourceLabel;
    HWND m_hRegExpLabel;
    HWND m_hReplaceLabel;
    HWND m_hFormatLabel;
    HWND m_hParametersLabel;
    HWND m_hDataLabel;
    
    HMENU m_hMenu;
    
    // RegExp engines
    std::unique_ptr<RegExpEngine> m_pRegExpEngine;
    
    // Application state
    AppMode m_currentMode;
    RegExpEngineType m_currentEngine;
    int m_fontSize;
    int m_opacity;
    bool m_alwaysOnTop;
    bool m_uiReady;
    
    // Window state (deprecated - use per-mode state)
    int m_windowX;
    int m_windowY;
    int m_windowWidth;
    int m_windowHeight;
    
    // Window state for each mode
    struct WindowState {
        int x;
        int y;
        int width;
        int height;
    };
    WindowState m_regexpWindowState;
    WindowState m_printfWindowState;
    WindowState m_scanfWindowState;
    
    // History for each mode and field
    std::vector<std::string> m_sourceHistory;       // RegExp: Source
    std::vector<std::string> m_regexpHistory;       // RegExp: RegExp pattern
    std::vector<std::string> m_replaceHistory;      // RegExp: Replace
    std::vector<std::string> m_printfFormatHistory; // Printf: Format
    std::vector<std::string> m_parametersHistory;   // Printf: Parameters
    std::vector<std::string> m_scanfFormatHistory;  // Scanf: Format
    std::vector<std::string> m_dataHistory;         // Scanf: Data
    
    // Current field values for each mode
    std::string m_savedRegExpSource;
    std::string m_savedRegExpPattern;
    std::string m_savedRegExpReplace;
    std::string m_savedPrintfFormat;
    std::string m_savedPrintfParameters;
    std::string m_savedScanfFormat;
    std::string m_savedScanfData;
    
    // UI state
    int m_dpiX;
    int m_dpiY;
    
    // UI resources
    HBRUSH m_hWhiteBrush;
    HFONT m_hFont;
    WNDPROC m_originalInputProc;
    WNDPROC m_originalComboProc;

public:
    RegExpCalc();
    ~RegExpCalc();

    // Initialization
    BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
    ATOM RegisterClass(HINSTANCE hInstance);
    void Initialize(HWND hwnd);
    
    // Public accessors
    HWND GetMainWindow() const { return m_hWnd; }
    AppMode GetCurrentMode() const { return m_currentMode; }

    // Window procedures
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK ComboSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    // Message handlers
    void OnCreate();
    void OnSize(int width, int height);
    void OnMove(int x, int y);
    void OnCommand(WPARAM wParam);
    void OnPaint();
    void OnDestroy();
    void OnKeyDown(WPARAM key);

    // Evaluation operations
    void EvaluateExpression();
    void OnTextChanged();
    
    // Menu operations
    void UpdateMenuChecks();
    void UpdateWindowTitle();
    void OnMenuCommand(int command);
    void SetAppMode(AppMode mode);
    void SetRegExpEngine(RegExpEngineType engine);
    void CreateRegExpEngine(RegExpEngineType engine);
    
    // Hot key operations
    void OnCtrlHome(); // Reset to default position and settings
    void OnCtrlPlus(); // Increase font size
    void OnCtrlMinus(); // Decrease font size
    void OnCtrlShiftPlus(); // Increase opacity
    void OnCtrlShiftMinus(); // Decrease opacity
    
    // History management
    void LoadHistory();
    void SaveHistory();
    void AddToHistory(const std::string& text, AppMode mode, int fieldIndex);
    void LoadHistoryItem(int index, AppMode mode, int fieldIndex);
    void UpdateHistoryCombo(HWND hCombo, const std::vector<std::string>& history);
    void DeleteSelectedHistoryItem(HWND hCombo, std::vector<std::string>& history);
    void DeleteSelectedHistoryItem(HWND hCombo, AppMode mode, int fieldIndex);
    
    // Settings persistence
    void LoadSettings();
    void SaveSettings();
    void LoadCurrentValues();
    void SaveCurrentValues();
    std::string LoadStringFromRegistry(HKEY hKey, const wchar_t* valueName);
    
    // Mode field state management
    void SaveCurrentModeFields();
    void RestoreCurrentModeFields();
    
    // Window state management per mode
    void SaveCurrentWindowState();
    void RestoreCurrentWindowState();
    WindowState& GetWindowStateForMode(AppMode mode);

    // Layout management
    void InitializeDPI();
    int ScaleDPI(int value);
    void UpdateLayout();
    void UpdateLayoutForMode(AppMode mode);
    void UpdateFont();
    int GetCharWidth();
    int GetCharHeight();
    int GetControlHeight();
    int CalculateOptimalWidth(int charCount = 80);

    // Utility
    void ShowHelp();
    void ShowContextHelp();
    void SetWindowOpacity(int opacity);
    void SetFontSize(int size);
    void SetAlwaysOnTop(bool alwaysOnTop);
    
    // Accessors
    HWND GetWindow() const { return m_hWnd; }
    HMENU GetWindowMenu() const { return m_hMenu; }
    RegExpEngineType GetCurrentEngine() const { return m_currentEngine; }
    bool IsUIReady() const { return m_uiReady; }
    
    // Helper functions
    void ShowControlsForMode(AppMode mode);
    void HideAllControls();
    std::string GetCurrentText(AppMode mode, int fieldIndex);
    void SetCurrentText(AppMode mode, int fieldIndex, const std::string& text);
    void ClearResult();
    void AppendResult(const std::string& text);
    void SetResult(const std::string& text);
    
    // Evaluation functions for different modes
    void EvaluateRegExp();
    void EvaluatePrintf();
    void EvaluateScanf();
};