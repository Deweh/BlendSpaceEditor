#include "NodeBuilder.h"
#include "imgui_internal.h"

namespace Util
{
    NodeBuilder::NodeBuilder(ImTextureID texture, int textureWidth, int textureHeight) :
        HeaderTextureId(texture),
        HeaderTextureWidth(textureWidth),
        HeaderTextureHeight(textureHeight),
        CurrentNodeId(0),
        HasHeader(false)
    {
    }

    void NodeBuilder::Begin(ed::NodeId id)
    {
        HasHeader = false;
        HeaderMin = HeaderMax = ImVec2();

        ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
        ed::BeginNode(id);

        ImGui::PushID(id.AsPointer());
        CurrentNodeId = id;
    }

    void NodeBuilder::End()
    {
        ed::EndNode();
        ContentMin = ImGui::GetItemRectMin();
        ContentMax = ImGui::GetItemRectMax();

        if (ImGui::IsItemVisible())
        {
            auto alpha = static_cast<int>(120 * ImGui::GetStyle().Alpha);

            auto drawList = GetNodeBackgroundDrawList(CurrentNodeId);

            const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;

            auto headerColor = IM_COL32(0, 0, 0, alpha) | (HeaderColor & IM_COL32(255, 255, 255, 0));
            drawList->AddLine(
                ImVec2(HeaderMin.x - (8 - halfBorderWidth), HeaderMax.y - 0.5f),
                ImVec2(ContentMax.x + (8 - halfBorderWidth), HeaderMax.y - 0.5f),
                ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);

            drawList->AddRectFilled(
                ImVec2(HeaderMin.x - (8 - halfBorderWidth), HeaderMin.y - 3.0f),
                ImVec2(ContentMax.x - halfBorderWidth, HeaderMax.y - 0.5f),
                headerColor,
                ed::GetStyle().NodeRounding,
                ImDrawFlags_RoundCornersTop
            );

            /*
            if ((HeaderMax.x > HeaderMin.x) && (HeaderMax.y > HeaderMin.y))
            {
                if (HeaderTextureId) {
                    const auto uv = ImVec2(
                        (HeaderMax.x - HeaderMin.x) / (float)(4.0f * HeaderTextureWidth),
                        (HeaderMax.y - HeaderMin.y) / (float)(4.0f * HeaderTextureHeight));

                    drawList->AddImageRounded(HeaderTextureId,
                        ImVec2(HeaderMin.x - (8 - halfBorderWidth), HeaderMin.y - (4 - halfBorderWidth)),
                        ImVec2(HeaderMax.x + (8 - halfBorderWidth), HeaderMax.y),
                        ImVec2(0.0f, 0.0f), uv,
                        headerColor, ed::GetStyle().NodeRounding, ImDrawFlags_RoundCornersTop);
                }

                if (ContentMax.y > HeaderMax.y)
                {
                    drawList->AddLine(
                        ImVec2(HeaderMin.x - (8 - halfBorderWidth), HeaderMax.y - 0.5f),
                        ImVec2(HeaderMax.x + (8 - halfBorderWidth), HeaderMax.y - 0.5f),
                        ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
                }
            }
            */
        }

        CurrentNodeId = 0;

        ImGui::PopID();

        ed::PopStyleVar();
    }

    void NodeBuilder::BeginHeader(const ImVec4& color)
    {
        ImGui::Dummy(ImVec2(0, 4));
        HeaderMin = ImGui::GetItemRectMin();
        ImGui::Dummy(ImVec2(5, 0));
        ImGui::SameLine();
        HeaderColor = ImColor(color);
    }

    void NodeBuilder::EndHeader()
    {
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(5, 0));
        ImGui::Dummy(ImVec2(0, 4));
        
        HeaderMax = ImGui::GetItemRectMax();
        //ImGui::Dummy(ImVec2(0, ImGui::GetStyle().ItemSpacing.y * 2.0f));
    }

    void NodeBuilder::BeginLeft()
    {
        ImGui::BeginGroup();
    }

    void NodeBuilder::BeginInput(ed::PinId id)
    {
        ImGui::Dummy(ImVec2(0, 4));
        IsInput = true;
        Pin(id, ed::PinKind::Input);
    }

    void NodeBuilder::EndInput()
    {
        EndPin();
        IsInput = false;
    }

    void NodeBuilder::EndLeft()
    {
        ImGui::EndGroup();
    }

    void NodeBuilder::BeginRight()
    {
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(20, 0));
        ImGui::SameLine();
        ImGui::BeginGroup();
    }

    void NodeBuilder::BeginOutput(ed::PinId id, float leftPad)
    {
        ImGui::Dummy(ImVec2(0, 4));
        ImGui::Dummy(ImVec2(leftPad, 0));
        ImGui::SameLine();
        Pin(id, ed::PinKind::Output);
    }

    void NodeBuilder::EndOutput()
    {
        EndPin();
    }

    void NodeBuilder::EndRight()
    {
        ImGui::EndGroup();
    }

    void NodeBuilder::Pin(ed::PinId id, ed::PinKind kind)
    {
        ed::BeginPin(id, kind);
        ed::PinPivotAlignment(ImVec2(IsInput ? 0.f : 1.f, 0.5f));
        ed::PinPivotSize(ImVec2(0, 0));
    }

    void NodeBuilder::EndPin()
    {
        ed::EndPin();
    }
}