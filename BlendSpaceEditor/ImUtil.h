#pragma once
#include "Drawing.h"
#include <Windows.h>
#include <vector>

inline ImFont* ImGui_LoadWindowsFont(const char* fontName, float fontSize, ImGuiIO& io)
{
    // Create a temporary DC
    HDC hdc = CreateCompatibleDC(NULL);

    // Create the font
    HFONT hFont = CreateFont(
        -fontSize,                // Height (negative for character height)
        0, 0, 0,                  // Width, Escapement, Orientation
        FW_NORMAL,                // Weight
        FALSE, FALSE, FALSE,      // Italic, Underline, Strikeout
        DEFAULT_CHARSET,          // CharSet
        OUT_DEFAULT_PRECIS,       // OutPrecision
        CLIP_DEFAULT_PRECIS,      // ClipPrecision
        DEFAULT_QUALITY,          // Quality
        DEFAULT_PITCH | FF_DONTCARE, // Pitch and Family
        fontName);                // Face Name

    if (!hFont)
    {
        DeleteDC(hdc);
        return nullptr;
    }

    SelectObject(hdc, hFont);

    // Get font data size
    DWORD size = GetFontData(hdc, 0, 0, NULL, 0);
    if (size == GDI_ERROR)
    {
        DeleteObject(hFont);
        DeleteDC(hdc);
        return nullptr;
    }

    // Allocate memory and get the font data
    std::vector<BYTE> buffer(size);
    if (GetFontData(hdc, 0, 0, buffer.data(), size) == GDI_ERROR)
    {
        DeleteObject(hFont);
        DeleteDC(hdc);
        return nullptr;
    }

    // Clean up GDI objects
    DeleteObject(hFont);
    DeleteDC(hdc);

    // Add font to ImGui
    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;
    return io.Fonts->AddFontFromMemoryTTF(buffer.data(), size, fontSize, &fontConfig);
}

static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
    ImRect result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

void ImGui_Icon(const ImVec2& size, IconType type, bool filled, const ImVec4& color = ImVec4(1, 1, 1, 1), const ImVec4& innerColor = ImVec4(0, 0, 0, 0));