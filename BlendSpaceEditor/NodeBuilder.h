#pragma once
#include "imgui.h"
#include <imgui-node-editor/imgui_node_editor.h>

namespace Util
{
    namespace ed = ax::NodeEditor;

    struct NodeBuilder
    {
        NodeBuilder(ImTextureID texture = nullptr, int textureWidth = 0, int textureHeight = 0);

        void Begin(ed::NodeId id);
        void End();

        void BeginHeader(const ImVec4& color = ImVec4(1, 1, 1, 0.5));
        void EndHeader();

        void BeginLeft();

        void BeginInput(ed::PinId id);
        void EndInput();

        void EndLeft();
        void BeginRight();

        void BeginOutput(ed::PinId id, float leftPad = 0.0f);
        void EndOutput();

        void EndRight();

    private:
        void Pin(ed::PinId id, ax::NodeEditor::PinKind kind);
        void EndPin();

        bool IsInput = false;
        ImTextureID HeaderTextureId;
        int         HeaderTextureWidth;
        int         HeaderTextureHeight;
        ed::NodeId      CurrentNodeId;
        ImU32       HeaderColor;
        ImVec2      NodeMin;
        ImVec2      NodeMax;
        ImVec2      HeaderMin;
        ImVec2      HeaderMax;
        ImVec2      ContentMin;
        ImVec2      ContentMax;
        bool        HasHeader;
    };
}