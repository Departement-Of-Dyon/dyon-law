// ==========================================================================
// DyonLaw GUI — Departement-Of-Dyon · The Dyonian Legal Code
// Native Win32 app: document list, reader pane, full-text search.
// Build: g++ -O2 -mwindows -o lawwin.exe lawwin.cpp
// ==========================================================================
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "lawtexts.h"

#define IDC_LIST      1001
#define IDC_TEXT      1002
#define IDC_SEARCH    1003
#define IDC_SEARCHBTN 1004
#define IDC_STATUS    1005

static HWND hList, hText, hSearch, hSearchBtn, hStatus;
static HFONT hFont, hFontMono;
static int curDoc = -1;
static WNDPROC oldSearchProc;
static LRESULT CALLBACK SearchProc(HWND h, UINT m, WPARAM w, LPARAM l);

static void SetStatus(const char *s) {
    SetWindowTextA(hStatus, s);
}

static void ShowDoc(int idx) {
    if (idx < 0 || idx >= (int)NDOCS) return;
    curDoc = idx;
    // select in list
    SendMessageA(hList, LB_SETCURSEL, idx, 0);
    SetWindowTextA(hText, DOCS[idx].body);
    // scroll to top
    SendMessageA(hText, EM_SETSEL, 0, -1);
    SendMessageA(hText, EM_SCROLLCARET, 0, 0);
    SendMessageA(hText, EM_SETSEL, 0, 0);
    char buf[128];
    snprintf(buf, sizeof(buf), "%s — %s", DOCS[idx].title,
             idx < 5 ? "Book of the Code" : "Law");
    SetStatus(buf);
}

static void DoSearch(void) {
    char needle[256];
    GetWindowTextA(hSearch, needle, sizeof(needle));
    // trim + lowercase
    char *p = needle + strspn(needle, " \t");
    if (!*p) { SetStatus("type a search term first"); return; }
    for (char *c = p; *c; c++) *c = (char)tolower((unsigned char)*c);

    // build results into a buffer
    static char results[262144];
    results[0] = 0;
    int found = 0;
    size_t off = 0;
    for (size_t i = 0; i < NDOCS && found < 200; i++) {
        // lowercase copy of body
        size_t len = strlen(DOCS[i].body);
        char *low = (char *)malloc(len + 1);
        for (size_t j = 0; j <= len; j++)
            low[j] = (char)tolower((unsigned char)DOCS[i].body[j]);
        const char *hit = low;
        int lineNo = 0;
        while ((hit = strstr(hit, p)) && found < 200) {
            const char *ls = hit;
            while (ls > low && ls[-1] != '\n') ls--;
            const char *le = strchr(hit, '\n');
            int ll = le ? (int)(le - ls) : (int)strlen(ls);
            if (ll > 100) ll = 100;
            // grab matching line from original body at same offset
            size_t pos = (size_t)(ls - low);
            const char *ols = DOCS[i].body + pos;
            const char *ole = strchr(ols, '\n');
            int oll = ole ? (int)(ole - ols) : (int)strlen(ols);
            if (oll > 100) oll = 100;
            int n = snprintf(results + off, sizeof(results) - off,
                             "%s  —  %s\n  %.*s\n\n",
                             DOCS[i].title, "match", oll, ols);
            if (n < 0 || off + (size_t)n >= sizeof(results)) break;
            off += (size_t)n;
            found++;
            lineNo++;
            hit++;
        }
        free(low);
    }
    if (!found) {
        snprintf(results, sizeof(results), "No matches for \"%s\".\n", needle);
        SetWindowTextA(hText, results);
        SetStatus("no matches");
        return;
    }
    SetWindowTextA(hText, results);
    char buf[128];
    snprintf(buf, sizeof(buf), "%d match%s for \"%s\"", found, found == 1 ? "" : "es", needle);
    SetStatus(buf);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        hFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH, "Segoe UI");
        hFontMono = CreateFontA(15, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, FIXED_PITCH, "Consolas");

        // search row
        HWND lbl = CreateWindowA("STATIC", "Search:", WS_CHILD | WS_VISIBLE,
                                 10, 10, 50, 20, hwnd, NULL, NULL, NULL);
        SendMessageA(lbl, WM_SETFONT, (WPARAM)hFont, TRUE);
        hSearch = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                                62, 8, 320, 24, hwnd, (HMENU)IDC_SEARCH, NULL, NULL);
        SendMessageA(hSearch, WM_SETFONT, (WPARAM)hFont, TRUE);
        oldSearchProc = (WNDPROC)SetWindowLongPtrA(hSearch, GWLP_WNDPROC,
                                                 (LONG_PTR)SearchProc);
        hSearchBtn = CreateWindowA("BUTTON", "Search", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   390, 8, 80, 24, hwnd, (HMENU)IDC_SEARCHBTN, NULL, NULL);
        SendMessageA(hSearchBtn, WM_SETFONT, (WPARAM)hFont, TRUE);

        // document list
        hList = CreateWindowA("LISTBOX", "", WS_CHILD | WS_VISIBLE | WS_BORDER |
                              LBS_NOTIFY | WS_VSCROLL | LBS_HASSTRINGS,
                              10, 42, 270, 600, hwnd, (HMENU)IDC_LIST, NULL, NULL);
        SendMessageA(hList, WM_SETFONT, (WPARAM)hFont, TRUE);
        for (size_t i = 0; i < NDOCS; i++)
            SendMessageA(hList, LB_ADDSTRING, 0, (LPARAM)DOCS[i].title);
        SendMessageA(hList, LB_SETCURSEL, 0, 0);

        // text pane
        hText = CreateWindowA("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER |
                              ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL,
                              290, 42, 700, 600, hwnd, (HMENU)IDC_TEXT, NULL, NULL);
        SendMessageA(hText, WM_SETFONT, (WPARAM)hFontMono, TRUE);

        // status bar
        hStatus = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE | SS_SUNKEN,
                                10, 650, 980, 22, hwnd, (HMENU)IDC_STATUS, NULL, NULL);
        SendMessageA(hStatus, WM_SETFONT, (WPARAM)hFont, TRUE);
        ShowDoc(0);
        return 0;
    }
    case WM_SIZE: {
        int W = LOWORD(lp), H = HIWORD(lp);
        MoveWindow(hSearch, 62, 8, W - 500, 24, TRUE);
        MoveWindow(hSearchBtn, W - 105, 8, 90, 24, TRUE);
        MoveWindow(hList, 10, 42, 270, H - 115, TRUE);
        MoveWindow(hText, 290, 42, W - 300, H - 115, TRUE);
        MoveWindow(hStatus, 10, H - 30, W - 20, 22, TRUE);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == IDC_LIST && HIWORD(wp) == LBN_SELCHANGE) {
            int sel = (int)SendMessageA(hList, LB_GETCURSEL, 0, 0);
            if (sel >= 0) ShowDoc(sel);
        } else if (LOWORD(wp) == IDC_SEARCHBTN ||
                   (LOWORD(wp) == IDC_SEARCH && HIWORD(wp) == EN_CHANGE)) {
            // search on button; Enter triggers via EN_? keep simple
        } else if (LOWORD(wp) == IDC_SEARCH && HIWORD(wp) == EN_SETFOCUS) {
            // nothing
        }
        if (LOWORD(wp) == IDC_SEARCHBTN) DoSearch();
        // Enter in search box -> search
        if (LOWORD(wp) == IDC_SEARCH && HIWORD(wp) == EN_CHANGE) { /* noop */ }
        return 0;
    case WM_NOTIFY:
        return 0;
    case WM_GETDLGCODE:
        return DLGC_WANTCHARS;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

// subclass search box for Enter key
static LRESULT CALLBACK SearchProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_KEYDOWN && w == VK_RETURN) { DoSearch(); return 0; }
    return CallWindowProcA(oldSearchProc, h, m, w, l);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    WNDCLASSEXA wc = { sizeof(wc) };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "DyonLawWnd";
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassExA(&wc);

    HWND hwnd = CreateWindowExA(0, "DyonLawWnd", "DyonLaw — The Dyonian Legal Code",
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT, 1040, 720,
                                NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
