#include <Windows.h>
#include <string>

std::wstring Win32Util_OpenFileDialog(bool isSaveDialog, HWND owner = NULL, LPCWSTR filter = L"All Files (*.*)\0*.*\0") {
    OPENFILENAMEW ofn;
    wchar_t szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (isSaveDialog) {
        if (GetSaveFileNameW(&ofn) == TRUE) {
            return ofn.lpstrFile;
        }
    }
    else {
        if (GetOpenFileNameW(&ofn) == TRUE) {
            return ofn.lpstrFile;
        }
    }

    return L"";
}