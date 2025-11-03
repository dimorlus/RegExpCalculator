//#define NOMINMAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "pch.h"
#include "RegExpCalc.h"
#include "Resource.h"
#include "RegExpEngine.h"

#define BUFSIZE         4096
#define ARGSIZE         128
#define MAXSTRLEN       1024

#pragma warning(disable: 4996) // Disable deprecated function warnings

//---------------------------------------------------------------------------
//https://bbs.archlinux.org/viewtopic.php?id=31087
//scanf
//int main() {
//  char* m = (char*) malloc(sizeof(int)*2 + sizeof(char*)); /* prepare enough memory*/
//  void* bm = m; /* copies the pointer */
//  char* string = "I am a string!!"; /* an example string */
//
//  (*(int*)m) = 10; /*puts the first value */
//  m += sizeof(int); /* move forward the pointer to the next element */
//
//  (*(char**)m) = string; /* puts the next value */
//  m += sizeof(char*); /* move forward again*/
//
//  (*(int*)m) = 20; /* puts the third element */
//  m += sizeof(int); /* unneeded, but here for clarity. */
//
//  vprintf("%d %s %d\n", bm); /* the deep magic starts here...*/
//  free(bm);
//}
//---------------------------------------------------------------------------
//#define _LLM_CODE_
#define _DYNAMIC_
//sscanf test utility function 
int ssctst (char *dst, const char *sfmt, const char *str)
{
 char c = '\0';
 char cc = '\0';
 int ii = 0;
 int n = 0;
 int pfsize = 0;
 bool percent, open;
 enum ftypes
 {
  tNone, tChar, tShort, tInt, tLong, ti64, tFloat, tDouble, tExt, tString
 } fmt;

#ifdef _DYNAMIC_
  void **arglist = NULL;
  char *typelist = NULL;
  char *fmtlist = NULL;
  char** pstr = NULL;
#else //_DYNAMIC_
 void* arglist[ARGSIZE] = { NULL };
 char typelist[ARGSIZE] = { 0 };
 char fmtlist[ARGSIZE] = { 0 };
 char* pstr[ARGSIZE] = { NULL };
 memset(arglist, 0, sizeof(arglist));
 memset(typelist, 0, sizeof(typelist));
 memset(fmtlist, 0, sizeof(fmtlist));
 memset(pstr, 0, sizeof(pstr));
#endif //_DYNAMIC_

#ifdef _DYNAMIC_
 //make here 2 pass for count args and allocate memory
 for(int pass=0; pass<2; pass++)
#endif //_DYNAMIC_
 {
#ifdef _DYNAMIC_
  if (pass == 1)
   {
       arglist = (void**)malloc((n + 1) * sizeof(void*));
       typelist = (char*)malloc((n + 1) * sizeof(char));
       fmtlist = (char*)malloc((n + 1) * sizeof(char));
       pstr = (char**)malloc((n + 1) * sizeof(char*));
       if (arglist) memset(arglist, 0, (n + 1) * sizeof(void*));
       else return -1;
       if (typelist) memset(typelist, 0, (n + 1) * sizeof(char));
       else return -1;
       if (fmtlist) memset(fmtlist, 0, (n + 1) * sizeof(char));
       else return -1;
       if (pstr) memset(pstr, 0, (n + 1) * sizeof(char*));
       else return -1;
       c = cc = '\0';
       ii = n = pfsize = 0;
   }
#endif //_DYNAMIC_
     do
      {
       open = false;
       percent = false;
       fmt = tNone;
       do
	    {
	     cc = c;
	     c = sfmt[ii++];
	     if (open)
	      {
	       if ((c == ']'))
		    {
		     fmt = tString;
		     c = 's';
		     open = false;
		     break;
		    }
	       else continue;
	      }
	     if (percent)
	      {
	       if (c == '%')
		    {
		     percent = false;
		     continue;
		    }
	       else
	       if (c == '*')
		    {
		     fmt = tNone;
		     percent = false;
		     continue;
		    }
	       else
	       if ((c == 'f') || (c == 'e') || (c == 'E') || (c == 'g') || (c == 'G'))
		    {
		     fmt = tFloat;
		     if (cc == 'l') fmt = tDouble;
		     else
		     if (cc == 'L') fmt = tExt;
		     break;
		    }
	       else
	       if ((c == 'd') || (c == 'i') || (c == 'u') ||
	           (c == 'x') || (c == 'X') || (c == 'o'))
		    {
		     fmt = tInt;
		     if (cc == 'L') fmt = ti64;
		     else 
                     if (cc == 'l')  fmt = tLong;
		     else 
                     if (cc == 'h')  fmt = tShort;
		     break;
		    }
	       else
	       if ((c == 'D') || (c == 'I') || (c == 'U') || (c == 'O'))
		    {
		     fmt = tLong;
		     c = tolower(c);
		     break;
		    }
	       else
	       if (c == 'c')
		    {
		     //fmt = tChar;
		     fmt = tString;
		     break;
		    }
	       else
	       if (c == 's')
		    {
		     fmt = tString;
		     break;
		    }
	       else
	       if (c == '[')
		    {
		     open = true;
		     continue;
		    }
	      }
	     else
	     if (c == '%')
	      {
	       percent = true;
	       continue;
	      }
	    }
       while (c);
       if (c && percent && fmt)
       {
#ifdef _DYNAMIC_
	       if (pass == 1)
#endif //_DYNAMIC_
           {
               typelist[n] = fmt;
               fmtlist[n] = c;
               pstr[n] = NULL;
               switch (fmt)
               {
               case tChar:
                   arglist[n] = (void*)malloc(sizeof(char));
                   *(char*)arglist[n] = '\0';
                   pfsize += sizeof(char);
                   break;
               case tShort:
                   arglist[n] = (void*)malloc(sizeof(short));
                   *(short*)arglist[n] = 0;
                   pfsize += sizeof(short);
                   break;
               case tInt:
                   arglist[n] = (void*)malloc(sizeof(int));
                   *(int*)arglist[n] = 0;
                   pfsize += sizeof(int);
                   break;
               case tLong:
                   arglist[n] = (void*)malloc(sizeof(long));
                   *(long*)arglist[n] = 0;
                   pfsize += sizeof(long);
                   break;
               case ti64:
                   arglist[n] = (void*)malloc(sizeof(__int64));
                   *(__int64*)arglist[n] = 0;
                   pfsize += sizeof(__int64);
                   break;
               case tFloat:
                   arglist[n] = (void*)malloc(sizeof(float));
                   *(float*)arglist[n] = 0.0;
                   pfsize += sizeof(float);
                   break;
               case tDouble:
                   arglist[n] = (void*)malloc(sizeof(double));
                   *(double*)arglist[n] = 0.0;
                   pfsize += sizeof(double);
                   break;
               case tExt:
                   arglist[n] = (void*)malloc(sizeof(long double));
                   *(long double*)arglist[n] = 0.0;
                   pfsize += sizeof(long double);
                   break;
               case tString:
                   char* cp = (char*)malloc(BUFSIZE);
                   arglist[n] = (void*)malloc(sizeof(char*));
                   *(char**)arglist[n] = cp;
                   pstr[n] = cp;
                   memset(*(char**)arglist[n], 0, BUFSIZE);
                   pfsize += sizeof(char*);
                   break;
               }
           }
	       n++;
          }
        } while (c && (n < BUFSIZE));
   }//end for pass

    arglist[n] = (void*)malloc(sizeof(void*));

    int nn = vsscanf(str, sfmt, (char*)arglist);

    char* fdest = dst;
    for (int i = 0; (i < n) && arglist[i]; i++)
    {
        switch (typelist[i])
        {
        case tChar:
        {
            char ci = *(char*)arglist[i];
            char fmt[] = { "%c, \0" };
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, ci);
        }
        break;
        case tShort:
        {
            short si = *(short*)arglist[i];
            char fmt[] = { "%hd, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, si);
        }
        break;
        case tInt:
        {
            int ii = *(int*)arglist[i];
            char fmt[] = { "%d, \0" };
            fmt[1] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, ii);
        }
        break;
        case tLong:
        {
            long li = *(long*)arglist[i];
            char fmt[] = { "%ld, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, li);
        }
        break;
        case ti64:
        {
            __int64 Li = *(__int64*)arglist[i];
            char fmt[] = { "%Ld, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, Li);
        }
        break;
        case tFloat:
        {
            float ff = *(float*)arglist[i];
            char fmt[] = { "%f, \0" };
            fmt[1] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, ff);
        }
        break;
        case tDouble:
        {
            double df = *(double*)arglist[i];
            char fmt[] = { "%lf, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, df);
        }
        break;
        case tExt:
        {
            long double lf = *(long double*)arglist[i];
            char fmt[] = { "%Lf, \0" };
            fmt[2] = fmtlist[i];
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, lf);
        }
        break;
        case tString:
        {
            char* ss = (char*)arglist[i];
            char fmt[] = { "%s, \0" };
            fdest += snprintf(fdest, BUFSIZE - (fdest - dst), fmt, ss);
        }
        break;
        }
    }
    if (fdest > dst + 2) fdest[-2] = '\0'; //supress last ", "

#ifdef _DYNAMIC_
    for (int i = 0; i < n; i++)
#else //_DYNAMIC_
    for (int i = 0; i < ARGSIZE; i++)
#endif //_DYNAMIC_
    {
        if (pstr[i])
        {
            free(pstr[i]);
            pstr[i] = NULL;
        }
        if (arglist[i])
        {
            free(arglist[i]);
            arglist[i] = NULL;
        }
    }
#ifdef _DYNAMIC_
	 if (arglist) free(arglist);
     if (typelist) free(typelist);
     if (fmtlist) free(fmtlist);
     if (pstr) free(pstr);
#endif //_DYNAMIC_   
    return nn;
}
//---------------------------------------------------------------------------
//sprintf test utility function
void sprtst(char* dst, char* sfmt, char* str)
{
    char pfmt[BUFSIZE];
    char pstr[BUFSIZE];
    enum ftypes {
        tNone, tShort, tInt, tLong, tInt64,
        tFloat, tDouble, tLongDouble, tString, tPtr
    } fmt;
    char c = '\0', cc = '\0';
    int i;
    bool flag;

    char* pdst = dst; // Новый указатель для отслеживания позиции в dst

    do
    {
        i = 0; 
        flag = false; 
        fmt = tNone;
        do
        {
            cc = c;
            c = pfmt[i++] = *sfmt++;
            pfmt[i] = '\0';
            if (c == '\0')  break;
            if (flag)
            {
                if (c == '%') 
                 { 
                    flag = false; 
                    continue; 
                 }
                else
                if ((c == 'f') || (c == 'e') || (c == 'E') || (c == 'g') || (c == 'G'))
                {
                    if (cc == 'l') fmt = tDouble;
                    else
                    if (cc == 'L') fmt = tLongDouble;
                    else fmt = tFloat;
                    break;
                }
                else
                if ((c == 'd') || (c == 'i') || (c == 'u') ||
                    (c == 'x') || (c == 'X') || (c == 'o') || (c == 'c'))
                {
                    if (cc == 'l') fmt = tLong;
                    else
                    if (cc == 'h') fmt = tShort;
                    else
                    if (cc == 'L') fmt = tInt64;
                    else fmt = tInt;
                    break;
                }
                else
                if ((c == 'n') || (c == 'p')) 
                 { 
                    fmt = tPtr; 
                    break; 
                 }
                else
                if (c == 's') 
                 { 
                    fmt = tString; 
                    break; 
                 }
                else continue;
            }
            else
            if (c == '%') 
             { 
                flag = true; 
                continue; 
             }
        } while (c && (i < BUFSIZE));
        if (!c) sfmt--; //poit to '\0'
        i = 0; flag = false;
        do
        {
            c = *str++;
            if (!flag && (c == '"')) 
            { 
                flag = true; 
                continue; 
            }
            if (flag && (c == '"')) 
            { 
                flag = false; 
                continue; 
            }
            if (!flag && ((c == ',') || (c == '\0'))) //add asterix
            {
                i = pstr[i] = '\0';
                switch (fmt)
                {
                case tNone:
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt);
                    //pdst += sprintf(pdst, pfmt);
                    break;
                case tPtr:
                    strcpy_s(pdst, BUFSIZE - (pdst - dst), pfmt);
                    pdst += strlen(pfmt);
                    break;
                case tInt:
                {
                    int l = atol(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, l);
                }
                break;
                case tLong:
                {
                    long l = atol(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, l);
                }
                break;
                case tInt64:
                {
                    __int64 l = _atoi64(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, l);
                }
                break;
                case tShort:
                {
                    short l = (short)atol(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, l);
                }
                break;
                case tFloat:
                {
                    float d = (float)atof(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, d);
                }
                break;
                case tDouble:
                {
                    double d = (double)atof(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, d);
                }
                break;
                case tLongDouble:
                {
                    long double d = (long double)atof(pstr);
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, d);
                }
                break;
                case tString:
                    pdst += snprintf(pdst, BUFSIZE - (pdst - dst), pfmt, pstr);
                    break;
                }
                break;
            }
            else pstr[i++] = c;
        } while (c && (i < BUFSIZE));
    } while (*sfmt && (i < BUFSIZE));
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


// ====== Утилита для подсчета строк в результате ======
int CountLines(const std::string& s) {
    return static_cast<int>(std::count(s.begin(), s.end(), '\n')) + 1;
}

// ========== Класс ===============

// Window procedure for the main window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static RegExpCalc* pCalc = nullptr;

    switch (uMsg)
    {
    case WM_CREATE:
    {
        pCalc = new RegExpCalc();
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCalc);
        pCalc->Initialize(hwnd);
        return 0;
    }

    case WM_COMMAND:
    {
        if (pCalc)
        {
            WORD notificationCode = HIWORD(wParam);
            HWND hCtrl = (HWND)lParam;

            // Handle combo box dropdown opening (only if lParam != 0, meaning it's from a control)
            if (hCtrl && notificationCode == CBN_DROPDOWN)
            {
                // Add current text to history when dropdown opens
                int len = GetWindowTextLengthW(hCtrl);
                if (len > 0)
                {
                    std::wstring wtext(len, L'\0');
                    GetWindowTextW(hCtrl, &wtext[0], len + 1);
                    
                    // Convert to UTF-8
                    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string currentText;
                    if (utf8Len > 0)
                    {
                        currentText.resize(utf8Len - 1);
                        WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, &currentText[0], utf8Len, nullptr, nullptr);
                    }
                    
                    if (!currentText.empty())
                    {
                        int ctrlId = GetDlgCtrlID(hCtrl);
                        int fieldIndex = 0;
                        
                        switch (ctrlId)
                        {
                        case IDC_SOURCE_EDIT: fieldIndex = 0; break;
                        case IDC_REGEXP_EDIT: fieldIndex = 1; break;
                        case IDC_REPLACE_EDIT: fieldIndex = 2; break;
                        case IDC_FORMAT_EDIT: fieldIndex = 0; break;
                        case IDC_PARAMETERS_EDIT: fieldIndex = 1; break;
                        case IDC_DATA_EDIT: fieldIndex = 1; break;
                        }
                        
                        pCalc->AddToHistory(currentText, pCalc->GetCurrentMode(), fieldIndex);
                    }
                }
                return 0;
            }
            
            // Handle combo box selection changes (only if lParam != 0, meaning it's from a control)
            if (hCtrl && notificationCode == CBN_SELCHANGE)
            {
                int selection = (int)SendMessage(hCtrl, CB_GETCURSEL, 0, 0);
                if (selection != CB_ERR)
                {
                    // Determine which combo box and load appropriate history item
                    int ctrlId = GetDlgCtrlID(hCtrl);
                    pCalc->LoadHistoryItem(selection, pCalc->GetCurrentMode(), ctrlId);
                }
                return 0;
            }

            // Handle edit control changes (from ComboBox, only if lParam != 0)
            if (hCtrl && notificationCode == CBN_EDITCHANGE)
            {
                pCalc->OnTextChanged();
                return 0;
            }

            // Handle menu commands and accelerators
            pCalc->OnCommand(LOWORD(wParam));
        }
        break;
    }

    case WM_KEYDOWN:
        if (pCalc)
        {
            pCalc->OnKeyDown(wParam);
        }
        break;

    case WM_SIZE:
        if (pCalc)
        {
            pCalc->UpdateLayout();
        }
        break;

    case WM_MOVE:
        if (pCalc)
        {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            pCalc->OnMove(rect.left, rect.top);
        }
        break;

    case WM_PAINT:
        if (pCalc)
        {
            pCalc->OnPaint();
        }
        break;

    case WM_USER + 1:
        // Delayed initial evaluation after initialization
        if (pCalc)
        {
            pCalc->EvaluateExpression();
        }
        break;

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
        if (pCalc && lParam)
        {
            // Set white background and black text for all controls
            WORD ctrlId = GetDlgCtrlID((HWND)lParam);
            if (ctrlId != 0) // Valid control ID
            {
                SetTextColor((HDC)wParam, RGB(0, 0, 0));    // Black text
                SetBkColor((HDC)wParam, RGB(255, 255, 255)); // White background
                return (LRESULT)GetStockObject(WHITE_BRUSH);
            }
        }
        break;

    case WM_DESTROY:
        if (pCalc)
        {
            pCalc->OnDestroy();
            // Don't delete pCalc here - it's a static variable
            // The system will clean it up when the process exits
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}


// RegExpCalc implementation
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

RegExpCalc::~RegExpCalc()
{
    // Don't delete brushes and fonts here - they might be shared
    // The system will clean them up when the process exits
}

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
    
    // Get menu handle
    m_hMenu = GetMenu(hwnd);
    
    InitializeDPI();
    UpdateFont();
    
    // Create all controls
    OnCreate();
    
    // Load history and settings
    LoadHistory();
    LoadSettings();
    
    m_uiReady = true;
    
    // Apply loaded settings
    SetAlwaysOnTop(m_alwaysOnTop);
    SetWindowOpacity(m_opacity);
    
    // Update combo boxes with loaded history
    UpdateHistoryCombo(m_hSourceInput, m_sourceHistory);
    UpdateHistoryCombo(m_hRegExpInput, m_regexpHistory);
    UpdateHistoryCombo(m_hReplaceInput, m_replaceHistory);
    UpdateHistoryCombo(m_hFormatInput, m_currentMode == AppMode::PRINTF ? m_printfFormatHistory : m_scanfFormatHistory);
    UpdateHistoryCombo(m_hParametersInput, m_parametersHistory);
    UpdateHistoryCombo(m_hDataInput, m_dataHistory);
    
    // Load last entered values
    LoadCurrentValues();
    
    // Set window title based on current mode and engine
    UpdateWindowTitle();
    
    // Update menu checks for initial state
    UpdateMenuChecks();
    
    // Restore window state for current mode
    RestoreCurrentWindowState();
    
    // Update layout for current mode
    UpdateLayoutForMode(m_currentMode);
    
    // Restore field values after layout is ready
    RestoreCurrentModeFields();
    
    // Initial evaluation - delay it to ensure controls are updated
    PostMessage(m_hWnd, WM_USER + 1, 0, 0);
    
    // Set focus to first input field and remove selection from all fields
    HWND hFirstInput = nullptr;
    switch (m_currentMode)
    {
    case AppMode::REGEXP:
        hFirstInput = m_hSourceInput;
        // Remove selection from all RegExp fields
        PostMessage(m_hSourceInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        PostMessage(m_hRegExpInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        PostMessage(m_hReplaceInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        break;
    case AppMode::PRINTF:
        hFirstInput = m_hFormatInput;
        // Remove selection from all Printf fields
        PostMessage(m_hFormatInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        PostMessage(m_hParametersInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        break;
    case AppMode::SCANF:
        hFirstInput = m_hFormatInput;
        // Remove selection from all Scanf fields
        PostMessage(m_hFormatInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        PostMessage(m_hDataInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        break;
    }
    if (hFirstInput)
    {
        SetFocus(hFirstInput);
    }
}

void RegExpCalc::OnCreate()
{
    // Create labels
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
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 200, m_hWnd, (HMENU)IDC_SOURCE_EDIT, m_hInst, nullptr);

    m_hRegExpInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 200, m_hWnd, (HMENU)IDC_REGEXP_EDIT, m_hInst, nullptr);

    m_hReplaceInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 200, m_hWnd, (HMENU)IDC_REPLACE_EDIT, m_hInst, nullptr);

    m_hFormatInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 200, m_hWnd, (HMENU)IDC_FORMAT_EDIT, m_hInst, nullptr);

    m_hParametersInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 200, m_hWnd, (HMENU)IDC_PARAMETERS_EDIT, m_hInst, nullptr);

    m_hDataInput = CreateWindowW(L"COMBOBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | CBS_DROPDOWN | CBS_AUTOHSCROLL,
        0, 0, 10, 200, m_hWnd, (HMENU)IDC_DATA_EDIT, m_hInst, nullptr);

    m_hResultEdit = CreateWindowW(L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_MULTILINE | ES_READONLY,
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

    // Subclass combo boxes for custom behavior
    m_originalComboProc = (WNDPROC)SetWindowLongPtr(m_hSourceInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hRegExpInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hReplaceInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hFormatInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hParametersInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
    SetWindowLongPtr(m_hDataInput, GWLP_WNDPROC, (LONG_PTR)ComboSubclassProc);
}

void RegExpCalc::OnSize(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
    UpdateLayout();
}

void RegExpCalc::OnMove(int x, int y)
{
    m_windowX = x;
    m_windowY = y;
}

void RegExpCalc::OnCommand(WPARAM wParam)
{
    int command = LOWORD(wParam);
    
    switch (command)
    {
    case ID_MODE_REGEXP:
        SetAppMode(AppMode::REGEXP);
        break;
    case ID_MODE_PRINTF:
        SetAppMode(AppMode::PRINTF);
        break;
    case ID_MODE_SCANF:
        SetAppMode(AppMode::SCANF);
        break;
    case ID_ENGINE_CLASSIC:
        SetRegExpEngine(RegExpEngineType::CLASSIC);
        break;
    case ID_ENGINE_MODERN:
        SetRegExpEngine(RegExpEngineType::MODERN);
        break;
    case ID_ENGINE_PYTHON:
        SetRegExpEngine(RegExpEngineType::PYTHON);
        break;
    case ID_ENGINE_RE2:
        SetRegExpEngine(RegExpEngineType::RE2);
        break;
    case ID_CALC_ALWAYSONTOP:
        SetAlwaysOnTop(!m_alwaysOnTop);
        break;
    case ID_CALC_EXIT:
        PostMessage(m_hWnd, WM_CLOSE, 0, 0);
        break;
    case IDM_ABOUT:
        DialogBox(m_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd, About);
        break;
    case IDM_CTRL_HOME:
        OnCtrlHome();
        break;
    case IDM_CTRL_PLUS:
        OnCtrlPlus();
        break;
    case IDM_CTRL_MINUS:
        OnCtrlMinus();
        break;
    case IDM_CTRL_SHIFT_PLUS:
        OnCtrlShiftPlus();
        break;
    case IDM_CTRL_SHIFT_MINUS:
        OnCtrlShiftMinus();
        break;
    }
    
    UpdateMenuChecks();
}

void RegExpCalc::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hWnd, &ps);
    EndPaint(m_hWnd, &ps);
}

void RegExpCalc::OnDestroy()
{
    SaveSettings();
    SaveHistory();
}

void RegExpCalc::OnKeyDown(WPARAM key)
{
    switch (key)
    {
    case VK_HOME:
        if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            OnCtrlHome();
        }
        break;
    case VK_OEM_PLUS:
    case VK_ADD:
        if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            if (GetKeyState(VK_SHIFT) & 0x8000)
            {
                OnCtrlShiftPlus();
            }
            else
            {
                OnCtrlPlus();
            }
        }
        break;
    case VK_OEM_MINUS:
    case VK_SUBTRACT:
        if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            if (GetKeyState(VK_SHIFT) & 0x8000)
            {
                OnCtrlShiftMinus();
            }
            else
            {
                OnCtrlMinus();
            }
        }
        break;
    }
}

void RegExpCalc::EvaluateExpression()
{
    if (!m_uiReady) return;
    
    ClearResult();
    
    switch (m_currentMode)
    {
    case AppMode::REGEXP:
        EvaluateRegExp();
        break;
    case AppMode::PRINTF:
        EvaluatePrintf();
        break;
    case AppMode::SCANF:
        EvaluateScanf();
        break;
    }
}

void RegExpCalc::OnTextChanged()
{
    // Update saved values when text changes
    SaveCurrentModeFields();
    EvaluateExpression();
}

void RegExpCalc::UpdateMenuChecks()
{
    // Update mode menu items
    CheckMenuItem(m_hMenu, ID_MODE_REGEXP, (m_currentMode == AppMode::REGEXP) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_MODE_PRINTF, (m_currentMode == AppMode::PRINTF) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_MODE_SCANF, (m_currentMode == AppMode::SCANF) ? MF_CHECKED : MF_UNCHECKED);
    
    // Update engine menu items
    CheckMenuItem(m_hMenu, ID_ENGINE_CLASSIC, (m_currentEngine == RegExpEngineType::CLASSIC) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_ENGINE_MODERN, (m_currentEngine == RegExpEngineType::MODERN) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_ENGINE_PYTHON, (m_currentEngine == RegExpEngineType::PYTHON) ? MF_CHECKED : MF_UNCHECKED);
    CheckMenuItem(m_hMenu, ID_ENGINE_RE2, (m_currentEngine == RegExpEngineType::RE2) ? MF_CHECKED : MF_UNCHECKED);
    
    // Update other menu items
    CheckMenuItem(m_hMenu, ID_CALC_ALWAYSONTOP, m_alwaysOnTop ? MF_CHECKED : MF_UNCHECKED);
}

void RegExpCalc::UpdateWindowTitle()
{
    std::wstring title;
    
    // Get mode name
    if (m_currentMode == AppMode::PRINTF)
        title = L"Printf Calculator";
    else if (m_currentMode == AppMode::SCANF)
        title = L"Scanf Calculator";
    else
        title = L"RegExp Calculator";
    
    // Add engine name for RegExp mode
    if (m_currentMode == AppMode::REGEXP && m_pRegExpEngine)
    {
        std::string engineName = m_pRegExpEngine->GetName();
        std::wstring wEngineName(engineName.begin(), engineName.end());
        title = wEngineName + L" - " + title;
    }
    
    SetWindowTextW(m_hWnd, title.c_str());
}

void RegExpCalc::SetAppMode(AppMode mode)
{
    if (m_currentMode == mode) return;
    
    // Save current mode fields and window state before switching
    SaveCurrentModeFields();
    SaveCurrentWindowState();
    
    m_currentMode = mode;
    
    UpdateMenuChecks();
    UpdateWindowTitle();
    
    // Update history for Format combo based on new mode
    if (mode == AppMode::PRINTF || mode == AppMode::SCANF)
    {
        UpdateHistoryCombo(m_hFormatInput, mode == AppMode::PRINTF ? m_printfFormatHistory : m_scanfFormatHistory);
    }
    
    // Restore window state for new mode
    RestoreCurrentWindowState();
    
    UpdateLayoutForMode(mode);
    
    // Restore fields for new mode
    RestoreCurrentModeFields();
    
    EvaluateExpression();
    
    // Force complete redraw to eliminate visual artifacts
    InvalidateRect(m_hWnd, nullptr, TRUE);
    UpdateWindow(m_hWnd);
}

void RegExpCalc::SetRegExpEngine(RegExpEngineType engine)
{
    if (m_currentEngine == engine) return;
    
    m_currentEngine = engine;
    CreateRegExpEngine(engine);
    UpdateMenuChecks();
    UpdateWindowTitle();
    EvaluateExpression();
}

void RegExpCalc::CreateRegExpEngine(RegExpEngineType engine)
{
    switch (engine)
    {
    case RegExpEngineType::CLASSIC:
        m_pRegExpEngine = std::make_unique<ClassicRegExpEngine>();
        break;
    case RegExpEngineType::MODERN:
        m_pRegExpEngine = std::make_unique<ModernRegExpEngine>();
        break;
    case RegExpEngineType::PYTHON:
        m_pRegExpEngine = std::make_unique<PythonRegExpEngine>();
        break;
    case RegExpEngineType::RE2:
        m_pRegExpEngine = std::make_unique<RE2Engine>();
        break;
    }
}

void RegExpCalc::OnCtrlHome()
{
    // Reset to default position, size and settings
    SetWindowPos(m_hWnd, HWND_TOP, 100, 100, 680, 350, SWP_NOZORDER);
    SetFontSize(DEFAULT_FONT_SIZE);
    SetWindowOpacity(100);
    SetAlwaysOnTop(false);
}

void RegExpCalc::OnCtrlPlus()
{
    if (m_fontSize < MAX_FONT_SIZE)
    {
        SetFontSize(m_fontSize + 1);
    }
}

void RegExpCalc::OnCtrlMinus()
{
    if (m_fontSize > MIN_FONT_SIZE)
    {
        SetFontSize(m_fontSize - 1);
    }
}

void RegExpCalc::OnCtrlShiftPlus()
{
    if (m_opacity < 100)
    {
        SetWindowOpacity(m_opacity + 10);
    }
}

void RegExpCalc::OnCtrlShiftMinus()
{
    if (m_opacity > 10)
    {
        SetWindowOpacity(m_opacity - 10);
    }
}

void RegExpCalc::LoadHistory()
{
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, 
        L"Software\\RegExpCalc\\History", 
        0, NULL, 0, KEY_READ, NULL, &hKey, NULL);
    
    if (result != ERROR_SUCCESS) return;
    
    // Helper lambda to load history for a field
    auto LoadFieldHistory = [&](const wchar_t* valueName, std::vector<std::string>& history) {
        DWORD dataSize = 0;
        result = RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &dataSize);
        if (result == ERROR_SUCCESS && dataSize > 0) {
            std::vector<wchar_t> buffer(dataSize / sizeof(wchar_t));
            result = RegQueryValueExW(hKey, valueName, NULL, NULL, 
                (LPBYTE)buffer.data(), &dataSize);
            if (result == ERROR_SUCCESS) {
                // Parse multi-string (REG_MULTI_SZ format)
                wchar_t* ptr = buffer.data();
                while (*ptr) {
                    std::wstring wstr(ptr);
                    // Convert wstring to string using WideCharToMultiByte
                    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    if (len > 0) {
                        std::string str(len - 1, '\0');
                        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);
                        history.push_back(str);
                    }
                    ptr += wstr.length() + 1;
                }
            }
        }
    };
    
    LoadFieldHistory(L"Source", m_sourceHistory);
    LoadFieldHistory(L"RegExp", m_regexpHistory);
    LoadFieldHistory(L"Replace", m_replaceHistory);
    LoadFieldHistory(L"PrintfFormat", m_printfFormatHistory);
    LoadFieldHistory(L"Parameters", m_parametersHistory);
    LoadFieldHistory(L"ScanfFormat", m_scanfFormatHistory);
    LoadFieldHistory(L"Data", m_dataHistory);
    
    RegCloseKey(hKey);
}

void RegExpCalc::SaveHistory()
{
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, 
        L"Software\\RegExpCalc\\History", 
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    
    if (result != ERROR_SUCCESS) return;
    
    // Helper lambda to save history for a field
    auto SaveFieldHistory = [&](const wchar_t* valueName, const std::vector<std::string>& history) {
        if (history.empty()) {
            RegDeleteValueW(hKey, valueName);
            return;
        }
        
        // Build multi-string buffer (REG_MULTI_SZ format)
        std::vector<wchar_t> buffer;
        for (const auto& item : history) {
            std::wstring witem(item.begin(), item.end());
            buffer.insert(buffer.end(), witem.begin(), witem.end());
            buffer.push_back(L'\0');
        }
        buffer.push_back(L'\0'); // Double null terminator
        
        RegSetValueExW(hKey, valueName, 0, REG_MULTI_SZ, 
            (LPBYTE)buffer.data(), (DWORD)(buffer.size() * sizeof(wchar_t)));
    };
    
    SaveFieldHistory(L"Source", m_sourceHistory);
    SaveFieldHistory(L"RegExp", m_regexpHistory);
    SaveFieldHistory(L"Replace", m_replaceHistory);
    SaveFieldHistory(L"PrintfFormat", m_printfFormatHistory);
    SaveFieldHistory(L"Parameters", m_parametersHistory);
    SaveFieldHistory(L"ScanfFormat", m_scanfFormatHistory);
    SaveFieldHistory(L"Data", m_dataHistory);
    
    RegCloseKey(hKey);
}

void RegExpCalc::AddToHistory(const std::string& text, AppMode mode, int fieldIndex)
{
    if (text.empty()) return;
    
    std::vector<std::string>* pHistory = nullptr;
    HWND hCombo = nullptr;
    
    // Determine which history vector and combo to use
    switch (mode)
    {
    case AppMode::REGEXP:
        switch (fieldIndex)
        {
        case 0: pHistory = &m_sourceHistory; hCombo = m_hSourceInput; break;
        case 1: pHistory = &m_regexpHistory; hCombo = m_hRegExpInput; break;
        case 2: pHistory = &m_replaceHistory; hCombo = m_hReplaceInput; break;
        }
        break;
    case AppMode::PRINTF:
        switch (fieldIndex)
        {
        case 0: pHistory = &m_printfFormatHistory; hCombo = m_hFormatInput; break;
        case 1: pHistory = &m_parametersHistory; hCombo = m_hParametersInput; break;
        }
        break;
    case AppMode::SCANF:
        switch (fieldIndex)
        {
        case 0: pHistory = &m_scanfFormatHistory; hCombo = m_hFormatInput; break;
        case 1: pHistory = &m_dataHistory; hCombo = m_hDataInput; break;
        }
        break;
    }
    
    if (!pHistory || !hCombo) return;
    
    // Remove duplicate if exists
    auto it = std::find(pHistory->begin(), pHistory->end(), text);
    if (it != pHistory->end()) {
        pHistory->erase(it);
    }
    
    // Add to front of history
    pHistory->insert(pHistory->begin(), text);
    
    // Limit history size
    if (pHistory->size() > MAX_HISTORY) {
        pHistory->resize(MAX_HISTORY);
    }
    
    // Update combo box
    UpdateHistoryCombo(hCombo, *pHistory);
    
    // Set current text properly
    int len = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    if (len > 0)
    {
        std::wstring wtext(len - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wtext[0], len);
        SetWindowTextW(hCombo, wtext.c_str());
    }
}

void RegExpCalc::LoadHistoryItem(int index, AppMode mode, int fieldIndex)
{
    std::vector<std::string>* pHistory = nullptr;
    
    // Determine which history vector to use
    switch (mode)
    {
    case AppMode::REGEXP:
        switch (fieldIndex)
        {
        case IDC_SOURCE_EDIT: pHistory = &m_sourceHistory; break;
        case IDC_REGEXP_EDIT: pHistory = &m_regexpHistory; break;
        case IDC_REPLACE_EDIT: pHistory = &m_replaceHistory; break;
        }
        break;
    case AppMode::PRINTF:
        switch (fieldIndex)
        {
        case IDC_FORMAT_EDIT: pHistory = &m_printfFormatHistory; break;
        case IDC_PARAMETERS_EDIT: pHistory = &m_parametersHistory; break;
        }
        break;
    case AppMode::SCANF:
        switch (fieldIndex)
        {
        case IDC_FORMAT_EDIT: pHistory = &m_scanfFormatHistory; break;
        case IDC_DATA_EDIT: pHistory = &m_dataHistory; break;
        }
        break;
    }
    
    if (!pHistory || index < 0 || index >= (int)pHistory->size())
    {
        return;
    }
    
    const std::string& selectedText = (*pHistory)[index];
    
    // CBN_SELCHANGE comes BEFORE the text is updated in the edit part
    // We need to explicitly save the selected text to our state
    switch (mode)
    {
    case AppMode::REGEXP:
        switch (fieldIndex)
        {
        case IDC_SOURCE_EDIT: m_savedRegExpSource = selectedText; break;
        case IDC_REGEXP_EDIT: m_savedRegExpPattern = selectedText; break;
        case IDC_REPLACE_EDIT: m_savedRegExpReplace = selectedText; break;
        }
        break;
    case AppMode::PRINTF:
        switch (fieldIndex)
        {
        case IDC_FORMAT_EDIT: m_savedPrintfFormat = selectedText; break;
        case IDC_PARAMETERS_EDIT: m_savedPrintfParameters = selectedText; break;
        }
        break;
    case AppMode::SCANF:
        switch (fieldIndex)
        {
        case IDC_FORMAT_EDIT: m_savedScanfFormat = selectedText; break;
        case IDC_DATA_EDIT: m_savedScanfData = selectedText; break;
        }
        break;
    }
    
    // Trigger evaluation - this will use the saved values
    EvaluateExpression();
}

void RegExpCalc::UpdateHistoryCombo(HWND hCombo, const std::vector<std::string>& history)
{
    if (!hCombo) return;
    
    SendMessage(hCombo, CB_RESETCONTENT, 0, 0);
    
    for (const auto& item : history)
    {
        if (item.empty()) continue;
        
        // Convert string to wstring properly
        int len = MultiByteToWideChar(CP_UTF8, 0, item.c_str(), -1, nullptr, 0);
        if (len > 0)
        {
            std::wstring witem(len - 1, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, item.c_str(), -1, &witem[0], len);
            SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)witem.c_str());
        }
    }
}

void RegExpCalc::DeleteSelectedHistoryItem(HWND hCombo, AppMode mode, int fieldIndex)
{
    int selection = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if (selection != CB_ERR)
    {
        SendMessage(hCombo, CB_DELETESTRING, selection, 0);
        // TODO: Remove from appropriate history vector
    }
}

void RegExpCalc::DeleteSelectedHistoryItem(HWND hCombo, std::vector<std::string>& history)
{
    int selection = (int)SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    if (selection != CB_ERR && selection < (int)history.size())
    {
        history.erase(history.begin() + selection);
        SendMessage(hCombo, CB_DELETESTRING, selection, 0);
    }
}

void RegExpCalc::LoadSettings()
{
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, 
        L"Software\\RegExpCalc\\Settings", 
        0, NULL, 0, KEY_READ, NULL, &hKey, NULL);
    
    if (result != ERROR_SUCCESS) return;
    
    DWORD dwValue;
    DWORD dwSize = sizeof(DWORD);
    
    // Load current mode
    if (RegQueryValueExW(hKey, L"CurrentMode", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
    {
        if (dwValue <= 2) m_currentMode = (AppMode)dwValue;
    }
    
    // Load current engine
    if (RegQueryValueExW(hKey, L"CurrentEngine", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
    {
        if (dwValue <= 3) // 0=CLASSIC, 1=MODERN, 2=PYTHON, 3=RE2
        {
            m_currentEngine = (RegExpEngineType)dwValue;
            CreateRegExpEngine(m_currentEngine);
        }
    }
    
    // Load font size
    if (RegQueryValueExW(hKey, L"FontSize", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
    {
        if (dwValue >= MIN_FONT_SIZE && dwValue <= MAX_FONT_SIZE)
            m_fontSize = (int)dwValue;
    }
    
    // Load opacity
    if (RegQueryValueExW(hKey, L"Opacity", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
    {
        if (dwValue >= 10 && dwValue <= 100)
            m_opacity = (int)dwValue;
    }
    
    // Load always on top
    if (RegQueryValueExW(hKey, L"AlwaysOnTop", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
    {
        m_alwaysOnTop = (dwValue != 0);
    }
    
    // Load window position and size for each mode
    if (RegQueryValueExW(hKey, L"RegExpWindowX", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_regexpWindowState.x = (int)dwValue;
    if (RegQueryValueExW(hKey, L"RegExpWindowY", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_regexpWindowState.y = (int)dwValue;
    if (RegQueryValueExW(hKey, L"RegExpWindowWidth", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_regexpWindowState.width = (int)dwValue;
    if (RegQueryValueExW(hKey, L"RegExpWindowHeight", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_regexpWindowState.height = (int)dwValue;
    
    if (RegQueryValueExW(hKey, L"PrintfWindowX", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_printfWindowState.x = (int)dwValue;
    if (RegQueryValueExW(hKey, L"PrintfWindowY", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_printfWindowState.y = (int)dwValue;
    if (RegQueryValueExW(hKey, L"PrintfWindowWidth", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_printfWindowState.width = (int)dwValue;
    if (RegQueryValueExW(hKey, L"PrintfWindowHeight", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_printfWindowState.height = (int)dwValue;
    
    if (RegQueryValueExW(hKey, L"ScanfWindowX", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_scanfWindowState.x = (int)dwValue;
    if (RegQueryValueExW(hKey, L"ScanfWindowY", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_scanfWindowState.y = (int)dwValue;
    if (RegQueryValueExW(hKey, L"ScanfWindowWidth", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_scanfWindowState.width = (int)dwValue;
    if (RegQueryValueExW(hKey, L"ScanfWindowHeight", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
        m_scanfWindowState.height = (int)dwValue;
    
    RegCloseKey(hKey);
}

std::string RegExpCalc::LoadStringFromRegistry(HKEY hKey, const wchar_t* valueName)
{
    DWORD dataSize = 0;
    LONG result = RegQueryValueExW(hKey, valueName, NULL, NULL, NULL, &dataSize);
    
    if (result == ERROR_SUCCESS && dataSize > 0)
    {
        std::vector<wchar_t> buffer(dataSize / sizeof(wchar_t));
        if (RegQueryValueExW(hKey, valueName, NULL, NULL, (LPBYTE)buffer.data(), &dataSize) == ERROR_SUCCESS)
        {
            // Convert to UTF-8
            int utf8Len = WideCharToMultiByte(CP_UTF8, 0, buffer.data(), -1, nullptr, 0, nullptr, nullptr);
            if (utf8Len > 0)
            {
                std::string resultStr(utf8Len - 1, '\0');
                WideCharToMultiByte(CP_UTF8, 0, buffer.data(), -1, &resultStr[0], utf8Len, nullptr, nullptr);
                return resultStr;
            }
        }
    }
    return "";
}

void RegExpCalc::LoadCurrentValues()
{
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, 
        L"Software\\RegExpCalc\\CurrentValues", 
        0, NULL, 0, KEY_READ, NULL, &hKey, NULL);
    
    if (result != ERROR_SUCCESS) return;
    
    // Load current values for each field into saved variables
    m_savedRegExpSource = LoadStringFromRegistry(hKey, L"Source");
    m_savedRegExpPattern = LoadStringFromRegistry(hKey, L"RegExp");
    m_savedRegExpReplace = LoadStringFromRegistry(hKey, L"Replace");
    m_savedPrintfFormat = LoadStringFromRegistry(hKey, L"PrintfFormat");
    m_savedPrintfParameters = LoadStringFromRegistry(hKey, L"Parameters");
    m_savedScanfFormat = LoadStringFromRegistry(hKey, L"ScanfFormat");
    m_savedScanfData = LoadStringFromRegistry(hKey, L"Data");
    
    RegCloseKey(hKey);
    
    // Apply loaded values to controls for current mode
    if (m_uiReady)
    {
        RestoreCurrentModeFields();
    }
}

void RegExpCalc::SaveSettings()
{
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, 
        L"Software\\RegExpCalc\\Settings", 
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    
    if (result != ERROR_SUCCESS) return;
    
    DWORD dwValue;
    
    // Save current mode
    dwValue = (DWORD)m_currentMode;
    RegSetValueExW(hKey, L"CurrentMode", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    
    // Save current engine
    dwValue = (DWORD)m_currentEngine;
    RegSetValueExW(hKey, L"CurrentEngine", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    
    // Save font size
    dwValue = (DWORD)m_fontSize;
    RegSetValueExW(hKey, L"FontSize", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    
    // Save opacity
    dwValue = (DWORD)m_opacity;
    RegSetValueExW(hKey, L"Opacity", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    
    // Save always on top
    dwValue = m_alwaysOnTop ? 1 : 0;
    RegSetValueExW(hKey, L"AlwaysOnTop", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    
    // Save current window state for current mode
    SaveCurrentWindowState();
    
    // Save window position and size for each mode
    dwValue = (DWORD)m_regexpWindowState.x;
    RegSetValueExW(hKey, L"RegExpWindowX", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_regexpWindowState.y;
    RegSetValueExW(hKey, L"RegExpWindowY", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_regexpWindowState.width;
    RegSetValueExW(hKey, L"RegExpWindowWidth", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_regexpWindowState.height;
    RegSetValueExW(hKey, L"RegExpWindowHeight", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    
    dwValue = (DWORD)m_printfWindowState.x;
    RegSetValueExW(hKey, L"PrintfWindowX", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_printfWindowState.y;
    RegSetValueExW(hKey, L"PrintfWindowY", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_printfWindowState.width;
    RegSetValueExW(hKey, L"PrintfWindowWidth", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_printfWindowState.height;
    RegSetValueExW(hKey, L"PrintfWindowHeight", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    
    dwValue = (DWORD)m_scanfWindowState.x;
    RegSetValueExW(hKey, L"ScanfWindowX", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_scanfWindowState.y;
    RegSetValueExW(hKey, L"ScanfWindowY", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_scanfWindowState.width;
    RegSetValueExW(hKey, L"ScanfWindowWidth", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    dwValue = (DWORD)m_scanfWindowState.height;
    RegSetValueExW(hKey, L"ScanfWindowHeight", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
    
    RegCloseKey(hKey);
    
    // Save current values
    SaveCurrentValues();
}

void RegExpCalc::SaveCurrentValues()
{
    // First save current mode fields
    SaveCurrentModeFields();
    
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, 
        L"Software\\RegExpCalc\\CurrentValues", 
        0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    
    if (result != ERROR_SUCCESS) return;
    
    // Helper lambda to save a string value
    auto SaveStringValue = [&](const wchar_t* valueName, const std::string& value) {
        if (value.empty())
        {
            RegDeleteValueW(hKey, valueName);
            return;
        }
        
        // Convert to wide string
        int len = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
        if (len > 0)
        {
            std::wstring wvalue(len - 1, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, &wvalue[0], len);
            RegSetValueExW(hKey, valueName, 0, REG_SZ, 
                (LPBYTE)wvalue.c_str(), (DWORD)((wvalue.length() + 1) * sizeof(wchar_t)));
        }
    };
    
    // Save all values from saved variables
    SaveStringValue(L"Source", m_savedRegExpSource);
    SaveStringValue(L"RegExp", m_savedRegExpPattern);
    SaveStringValue(L"Replace", m_savedRegExpReplace);
    SaveStringValue(L"PrintfFormat", m_savedPrintfFormat);
    SaveStringValue(L"Parameters", m_savedPrintfParameters);
    SaveStringValue(L"ScanfFormat", m_savedScanfFormat);
    SaveStringValue(L"Data", m_savedScanfData);
    
    RegCloseKey(hKey);
}

void RegExpCalc::SaveCurrentModeFields()
{
    // Save current field values based on current mode
    if (m_currentMode == AppMode::REGEXP)
    {
        m_savedRegExpSource = GetCurrentText(AppMode::REGEXP, 0);
        m_savedRegExpPattern = GetCurrentText(AppMode::REGEXP, 1);
        m_savedRegExpReplace = GetCurrentText(AppMode::REGEXP, 2);
    }
    else if (m_currentMode == AppMode::PRINTF)
    {
        m_savedPrintfFormat = GetCurrentText(AppMode::PRINTF, 0);
        m_savedPrintfParameters = GetCurrentText(AppMode::PRINTF, 1);
    }
    else if (m_currentMode == AppMode::SCANF)
    {
        m_savedScanfFormat = GetCurrentText(AppMode::SCANF, 0);
        m_savedScanfData = GetCurrentText(AppMode::SCANF, 1);
    }
}

void RegExpCalc::RestoreCurrentModeFields()
{
    // Restore field values for current mode without selecting text
    if (m_currentMode == AppMode::REGEXP)
    {
        SetCurrentText(AppMode::REGEXP, 0, m_savedRegExpSource);
        SendMessage(m_hSourceInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        
        SetCurrentText(AppMode::REGEXP, 1, m_savedRegExpPattern);
        SendMessage(m_hRegExpInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        
        SetCurrentText(AppMode::REGEXP, 2, m_savedRegExpReplace);
        SendMessage(m_hReplaceInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
    }
    else if (m_currentMode == AppMode::PRINTF)
    {
        SetCurrentText(AppMode::PRINTF, 0, m_savedPrintfFormat);
        SendMessage(m_hFormatInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        
        SetCurrentText(AppMode::PRINTF, 1, m_savedPrintfParameters);
        SendMessage(m_hParametersInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
    }
    else if (m_currentMode == AppMode::SCANF)
    {
        SetCurrentText(AppMode::SCANF, 0, m_savedScanfFormat);
        SendMessage(m_hFormatInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
        
        SetCurrentText(AppMode::SCANF, 1, m_savedScanfData);
        SendMessage(m_hDataInput, CB_SETEDITSEL, 0, MAKELPARAM(-1, 0));
    }
}

RegExpCalc::WindowState& RegExpCalc::GetWindowStateForMode(AppMode mode)
{
    switch (mode)
    {
    case AppMode::REGEXP:
        return m_regexpWindowState;
    case AppMode::PRINTF:
        return m_printfWindowState;
    case AppMode::SCANF:
        return m_scanfWindowState;
    default:
        return m_regexpWindowState;
    }
}

void RegExpCalc::SaveCurrentWindowState()
{
    RECT rect;
    GetWindowRect(m_hWnd, &rect);
    
    WindowState& state = GetWindowStateForMode(m_currentMode);
    state.x = rect.left;
    state.y = rect.top;
    state.width = rect.right - rect.left;
    state.height = rect.bottom - rect.top;
}

void RegExpCalc::RestoreCurrentWindowState()
{
    WindowState& state = GetWindowStateForMode(m_currentMode);
    
    // Only restore if we have saved values (non-zero width/height)
    if (state.width > 0 && state.height > 0)
    {
        SetWindowPos(m_hWnd, nullptr, 
                     state.x, state.y, 
                     state.width, state.height, 
                     SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

void RegExpCalc::InitializeDPI()
{
    HDC hdc = GetDC(m_hWnd);
    m_dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    m_dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(m_hWnd, hdc);
}

int RegExpCalc::ScaleDPI(int value)
{
    return MulDiv(value, m_dpiX, 96);
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

    // --- Result EditText занимает всю оставшуюся высоту ---
    int heightClient = rect.bottom - rect.top;
    int resultHeight = heightClient - y - BORDER_MARGIN;
    if (resultHeight < CONTROL_HEIGHT) resultHeight = CONTROL_HEIGHT;
    MoveWindow(m_hResultEdit, x, y, width - 2 * BORDER_MARGIN, resultHeight, TRUE);
}
void RegExpCalc::UpdateFont()
{
    if (m_hFont)
    {
        DeleteObject(m_hFont);
    }
    
    m_hFont = CreateFontW(
        ScaleDPI(m_fontSize), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, L"Courier New"
    );
    
    if (m_uiReady)
    {
        // Update fonts for all controls
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
        
        UpdateLayout();
    }
}

int RegExpCalc::GetCharWidth()
{
    HDC hdc = GetDC(m_hWnd);
    HFONT oldFont = (HFONT)SelectObject(hdc, m_hFont);
    
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    
    SelectObject(hdc, oldFont);
    ReleaseDC(m_hWnd, hdc);
    
    return tm.tmAveCharWidth;
}

int RegExpCalc::GetCharHeight()
{
    HDC hdc = GetDC(m_hWnd);
    HFONT oldFont = (HFONT)SelectObject(hdc, m_hFont);
    
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    
    SelectObject(hdc, oldFont);
    ReleaseDC(m_hWnd, hdc);
    
    return tm.tmHeight;
}

int RegExpCalc::GetControlHeight()
{
    return GetCharHeight() + ScaleDPI(8);
}

int RegExpCalc::CalculateOptimalWidth(int charCount)
{
    return GetCharWidth() * charCount + ScaleDPI(20);
}

void RegExpCalc::ShowHelp()
{
    // TODO: Show help file
}

void RegExpCalc::SetWindowOpacity(int opacity)
{
    m_opacity = opacity;
    
    if (opacity >= 100)
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
        SetLayeredWindowAttributes(m_hWnd, 0, 255, LWA_ALPHA);
    }
    else
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(m_hWnd, 0, (BYTE)(255 * opacity / 100), LWA_ALPHA);
    }
}

void RegExpCalc::SetFontSize(int size)
{
    if (size >= MIN_FONT_SIZE && size <= MAX_FONT_SIZE)
    {
        m_fontSize = size;
        UpdateFont();
    }
}

void RegExpCalc::SetAlwaysOnTop(bool alwaysOnTop)
{
    m_alwaysOnTop = alwaysOnTop;
    
    SetWindowPos(m_hWnd, 
        alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
        0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    
    UpdateMenuChecks();
}

void RegExpCalc::ShowControlsForMode(AppMode mode)
{
    // Hide all first
    HideAllControls();

    if (mode == AppMode::REGEXP)
    {
        if (m_hSourceLabel) ShowWindow(m_hSourceLabel, SW_SHOW);
        if (m_hSourceInput) ShowWindow(m_hSourceInput, SW_SHOW);
        if (m_hRegExpLabel) ShowWindow(m_hRegExpLabel, SW_SHOW);
        if (m_hRegExpInput) ShowWindow(m_hRegExpInput, SW_SHOW);
        if (m_hReplaceLabel) ShowWindow(m_hReplaceLabel, SW_SHOW);
        if (m_hReplaceInput) ShowWindow(m_hReplaceInput, SW_SHOW);
    }
    else if (mode == AppMode::PRINTF)
    {
        if (m_hFormatLabel) ShowWindow(m_hFormatLabel, SW_SHOW);
        if (m_hFormatInput) ShowWindow(m_hFormatInput, SW_SHOW);
        if (m_hParametersLabel) ShowWindow(m_hParametersLabel, SW_SHOW);
        if (m_hParametersInput) ShowWindow(m_hParametersInput, SW_SHOW);
    }
    else if (mode == AppMode::SCANF)
    {
        if (m_hFormatLabel) ShowWindow(m_hFormatLabel, SW_SHOW);
        if (m_hFormatInput) ShowWindow(m_hFormatInput, SW_SHOW);
        if (m_hDataLabel) ShowWindow(m_hDataLabel, SW_SHOW);
        if (m_hDataInput) ShowWindow(m_hDataInput, SW_SHOW);
    }

    if (m_hResultEdit) ShowWindow(m_hResultEdit, SW_SHOW);
    
    // Force redraw all labels
    if (m_hSourceLabel) InvalidateRect(m_hSourceLabel, nullptr, TRUE);
    if (m_hRegExpLabel) InvalidateRect(m_hRegExpLabel, nullptr, TRUE);
    if (m_hReplaceLabel) InvalidateRect(m_hReplaceLabel, nullptr, TRUE);
    if (m_hFormatLabel) InvalidateRect(m_hFormatLabel, nullptr, TRUE);
    if (m_hParametersLabel) InvalidateRect(m_hParametersLabel, nullptr, TRUE);
    if (m_hDataLabel) InvalidateRect(m_hDataLabel, nullptr, TRUE);
    
    // Force redraw
    if (m_hWnd)
    {
        InvalidateRect(m_hWnd, nullptr, TRUE);
        UpdateWindow(m_hWnd);
    }
}

void RegExpCalc::HideAllControls()
{
    if (m_hSourceLabel) ShowWindow(m_hSourceLabel, SW_HIDE);
    if (m_hSourceInput) ShowWindow(m_hSourceInput, SW_HIDE);
    if (m_hRegExpLabel) ShowWindow(m_hRegExpLabel, SW_HIDE);
    if (m_hRegExpInput) ShowWindow(m_hRegExpInput, SW_HIDE);
    if (m_hReplaceLabel) ShowWindow(m_hReplaceLabel, SW_HIDE);
    if (m_hReplaceInput) ShowWindow(m_hReplaceInput, SW_HIDE);
    if (m_hFormatLabel) ShowWindow(m_hFormatLabel, SW_HIDE);
    if (m_hFormatInput) ShowWindow(m_hFormatInput, SW_HIDE);
    if (m_hParametersLabel) ShowWindow(m_hParametersLabel, SW_HIDE);
    if (m_hParametersInput) ShowWindow(m_hParametersInput, SW_HIDE);
    if (m_hDataLabel) ShowWindow(m_hDataLabel, SW_HIDE);
    if (m_hDataInput) ShowWindow(m_hDataInput, SW_HIDE);
}

std::string RegExpCalc::GetCurrentText(AppMode mode, int fieldIndex)
{
    HWND hInput = nullptr;
    
    switch (mode)
    {
    case AppMode::REGEXP:
        switch (fieldIndex)
        {
        case 0: hInput = m_hSourceInput; break;
        case 1: hInput = m_hRegExpInput; break;
        case 2: hInput = m_hReplaceInput; break;
        }
        break;
    case AppMode::PRINTF:
        switch (fieldIndex)
        {
        case 0: hInput = m_hFormatInput; break;
        case 1: hInput = m_hParametersInput; break;
        }
        break;
    case AppMode::SCANF:
        switch (fieldIndex)
        {
        case 0: hInput = m_hFormatInput; break;
        case 1: hInput = m_hDataInput; break;
        }
        break;
    }
    
    if (hInput)
    {
        int length = GetWindowTextLengthW(hInput);
        if (length > 0)
        {
            std::wstring wtext(length, L'\0');
            GetWindowTextW(hInput, &wtext[0], length + 1);
            
            // Convert to UTF-8
            int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (utf8Len > 0)
            {
                std::string text(utf8Len - 1, '\0');
                WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, &text[0], utf8Len, nullptr, nullptr);
                return text;
            }
        }
    }
    
    return "";
}

void RegExpCalc::SetCurrentText(AppMode mode, int fieldIndex, const std::string& text)
{
    HWND hInput = nullptr;
    
    switch (mode)
    {
    case AppMode::REGEXP:
        switch (fieldIndex)
        {
        case 0: hInput = m_hSourceInput; break;
        case 1: hInput = m_hRegExpInput; break;
        case 2: hInput = m_hReplaceInput; break;
        }
        break;
    case AppMode::PRINTF:
        switch (fieldIndex)
        {
        case 0: hInput = m_hFormatInput; break;
        case 1: hInput = m_hParametersInput; break;
        }
        break;
    case AppMode::SCANF:
        switch (fieldIndex)
        {
        case 0: hInput = m_hFormatInput; break;
        case 1: hInput = m_hDataInput; break;
        }
        break;
    }
    
    if (hInput)
    {
        // First, clear any selection in the combo box
        SendMessage(hInput, CB_SETCURSEL, (WPARAM)-1, 0);
        
        int len = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
        if (len > 0)
        {
            std::wstring wtext(len - 1, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wtext[0], len);
            
            // For ComboBox, we need to set the text in the edit control part
            // SetWindowTextW sets the text in the edit portion of the combo
            SetWindowTextW(hInput, wtext.c_str());
        }
        else if (text.empty())
        {
            // Explicitly set empty text
            SetWindowTextW(hInput, L"");
        }
    }
}

void RegExpCalc::ClearResult()
{
    SetWindowTextA(m_hResultEdit, "");
}

void RegExpCalc::AppendResult(const std::string& text)
{
    int length = GetWindowTextLengthA(m_hResultEdit);
    SendMessageA(m_hResultEdit, EM_SETSEL, length, length);
    SendMessageA(m_hResultEdit, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
}

void RegExpCalc::SetResult(const std::string& text)
{
    SetWindowTextA(m_hResultEdit, text.c_str());
}

// Subclass procedures
LRESULT CALLBACK RegExpCalc::EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RegExpCalc* pCalc = nullptr;
    
    if (message == WM_KEYDOWN && wParam == VK_RETURN)
    {
        pCalc = (RegExpCalc*)GetWindowLongPtr(GetParent(hWnd), GWLP_USERDATA);
        if (pCalc)
        {
            pCalc->EvaluateExpression();
            return 0;
        }
    }
    
    return CallWindowProc(pCalc ? pCalc->m_originalInputProc : nullptr, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK RegExpCalc::ComboSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RegExpCalc* pCalc = (RegExpCalc*)GetWindowLongPtr(GetParent(hWnd), GWLP_USERDATA);
    
    if (pCalc)
    {
        // Add to history when focus is lost
        if (message == WM_KILLFOCUS)
        {
            int len = GetWindowTextLengthW(hWnd);
            if (len > 0)
            {
                std::wstring wtext(len, L'\0');
                GetWindowTextW(hWnd, &wtext[0], len + 1);
                
                // Convert to UTF-8
                int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, nullptr, 0, nullptr, nullptr);
                std::string currentText;
                if (utf8Len > 0)
                {
                    currentText.resize(utf8Len - 1);
                    WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, &currentText[0], utf8Len, nullptr, nullptr);
                }
                
                if (!currentText.empty())
                {
                    // Determine field index based on control ID
                    int ctrlId = GetDlgCtrlID(hWnd);
                    int fieldIndex = 0;
                    
                    switch (ctrlId)
                    {
                    case IDC_SOURCE_EDIT: fieldIndex = 0; break;
                    case IDC_REGEXP_EDIT: fieldIndex = 1; break;
                    case IDC_REPLACE_EDIT: fieldIndex = 2; break;
                    case IDC_FORMAT_EDIT: fieldIndex = 0; break;
                    case IDC_PARAMETERS_EDIT: fieldIndex = 1; break;
                    case IDC_DATA_EDIT: fieldIndex = 1; break;
                    }
                    
                    pCalc->AddToHistory(currentText, pCalc->GetCurrentMode(), fieldIndex);
                }
            }
        }
        
        if (message == WM_KEYDOWN)
        {
            if (wParam == VK_DELETE)
            {
                int ctrlId = GetDlgCtrlID(hWnd);
                pCalc->DeleteSelectedHistoryItem(hWnd, pCalc->GetCurrentMode(), ctrlId);
                return 0;
            }
            else if (wParam == VK_RETURN)
            {
                // Get text from this specific combo
                int len = GetWindowTextLengthW(hWnd);
                if (len > 0)
                {
                    std::wstring wtext(len, L'\0');
                    GetWindowTextW(hWnd, &wtext[0], len + 1);
                    
                    // Convert to UTF-8
                    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, nullptr, 0, nullptr, nullptr);
                    std::string currentText;
                    if (utf8Len > 0)
                    {
                        currentText.resize(utf8Len - 1);
                        WideCharToMultiByte(CP_UTF8, 0, wtext.c_str(), -1, &currentText[0], utf8Len, nullptr, nullptr);
                    }
                    
                    if (!currentText.empty())
                    {
                        // Determine field index based on control ID
                        int ctrlId = GetDlgCtrlID(hWnd);
                        int fieldIndex = 0;
                        
                        switch (ctrlId)
                        {
                        case IDC_SOURCE_EDIT: fieldIndex = 0; break;
                        case IDC_REGEXP_EDIT: fieldIndex = 1; break;
                        case IDC_REPLACE_EDIT: fieldIndex = 2; break;
                        case IDC_FORMAT_EDIT: fieldIndex = 0; break;
                        case IDC_PARAMETERS_EDIT: fieldIndex = 1; break;
                        case IDC_DATA_EDIT: fieldIndex = 1; break;
                        }
                        
                        pCalc->AddToHistory(currentText, pCalc->GetCurrentMode(), fieldIndex);
                    }
                }
                
                // Also trigger evaluation
                pCalc->EvaluateExpression();
            }
        }
    }
    
    return CallWindowProc(pCalc ? pCalc->m_originalComboProc : nullptr, hWnd, message, wParam, lParam);
}

// About dialog procedure
INT_PTR CALLBACK RegExpCalc::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// RegExp evaluation function
void RegExpCalc::EvaluateRegExp()
{
    // Use saved values instead of reading from controls
    std::string source = m_savedRegExpSource;
    std::string pattern = m_savedRegExpPattern;
    std::string replacement = m_savedRegExpReplace;
    
    if (pattern.empty())
    {
        SetResult("Res: Pattern is empty");
        return;
    }
    
    // Use selected RegExp engine
    if (!m_pRegExpEngine)
    {
        SetResult("Res: No RegExp engine available");
        return;
    }
    
    // Execute using selected engine
    RegExpResult engineResult = m_pRegExpEngine->Execute(source, pattern, replacement);
    
    if (engineResult.success)
    {
        SetResult(engineResult.result);
    }
    else
    {
        SetResult("Res: " + engineResult.error);
    }
}

void RegExpCalc::EvaluatePrintf()
{
    // Use saved values instead of reading from controls
    std::string format = m_savedPrintfFormat;
    std::string parameters = m_savedPrintfParameters;
    
    if (format.empty())
    {
        SetResult("Format string is empty");
        return;
    }
    
    char dst[BUFSIZE] = { '\0' };
    char fmtBuf[BUFSIZE];
    char paramBuf[BUFSIZE];
    
    strcpy_s(fmtBuf, BUFSIZE, format.c_str());
    strcpy_s(paramBuf, BUFSIZE, parameters.c_str());
    
    sprtst(dst, fmtBuf, paramBuf);
    
    char result[BUFSIZE * 2];
    snprintf(result, sizeof(result), "Res: %s", dst);
    
    SetResult(result);
}

void RegExpCalc::EvaluateScanf()
{
    // Use saved values instead of reading from controls
    // This is important because controls might not be updated yet (e.g., during CBN_SELCHANGE)
    std::string format = m_savedScanfFormat;
    std::string data = m_savedScanfData;
    
    if (format.empty())
    {
        SetResult("Format string is empty");
        return;
    }
    
    char dst[BUFSIZE] = { '\0' };
    char fmtBuf[BUFSIZE];
    char dataBuf[BUFSIZE];
    
    strcpy_s(fmtBuf, BUFSIZE, format.c_str());
    strcpy_s(dataBuf, BUFSIZE, data.c_str());
    
    int n = ssctst(dst, fmtBuf, dataBuf);
    
    char result[BUFSIZE * 2];
    snprintf(result, sizeof(result), "Res: %s", dst);
    
    SetResult(result);
}
