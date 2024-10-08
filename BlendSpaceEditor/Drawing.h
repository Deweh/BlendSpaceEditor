#pragma once

#include "imgui.h"
#include "imgui_internal.h"

enum class IconType : ImU32 { Flow, Circle, Square, Grid, RoundSquare, Diamond };

void Drawing_DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor);