#define IMGUI_DEFINE_MATH_OPERATORS
#include "Editor.h"
#include "imgui-node-editor/imgui_node_editor.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "NodeBuilder.h"
#include <array>
#include <iostream>
#undef max

namespace Main {
    extern ImFont* g_mainFont;
    extern ImFont* g_mainFontSmall;
    extern ImFont* g_mainFontMedium;
}

namespace ed = ax::NodeEditor;

Editor::Editor()
{
	ed::Config config;
    config.CanvasSizeMode = ed::CanvasSizeMode::CenterOnly;
    config.SettingsFile = "";
	m_Editor = ed::CreateEditor(&config);
    InitNew();
}

Editor::~Editor()
{
	ed::DestroyEditor(m_Editor);
}

void Editor::InitNew()
{
    m_Nodes.clear();
    m_Links.clear();

    ed::SetCurrentEditor(m_Editor);

    auto& defs = NodeDefinitions::GetDefList();
    for (auto& d : defs) {
        if (d->typeName == "anim") {
            auto node = SpawnNode(d);
            ed::SetNodePosition(node->id, ImVec2(16, 256));
        }
        else if (d->typeName == "actor") {
            auto node = SpawnNode(d);
            ed::SetNodePosition(node->id, ImVec2(1056, 256));
        }
    }

    ed::SetCurrentEditor(nullptr);
}

int Editor::GetNextId()
{
    return ++m_LastId;
}

Node* Editor::FindNode(ed::NodeId id)
{
    for (auto& node : m_Nodes)
        if (node.id == id)
            return &node;

    return nullptr;
}

Link* Editor::FindLink(ed::LinkId id)
{
    for (auto& link : m_Links)
        if (link.id == id)
            return &link;

    return nullptr;
}

Pin* Editor::FindPin(ed::PinId id)
{
    if (!id)
        return nullptr;

    for (auto& node : m_Nodes)
    {
        for (auto& pin : node.inputs)
            if (pin.id == id)
                return &pin;

        for (auto& pin : node.outputs)
            if (pin.id == id)
                return &pin;
    }

    return nullptr;
}

bool Editor::IsPinLinked(ed::PinId id)
{
    if (!id)
        return false;

    for (auto& link : m_Links)
        if (link.startPinID == id || link.endPinID == id)
            return true;

    return false;
}

bool Editor::CanCreateLink(Pin* a, Pin* b)
{
    if (!a || !b || a == b || a->kind == b->kind || a->type != b->type || a->node == b->node)
        return false;

    auto inputPin = a->kind == PinKind::Input ? a : b;
    auto outputPin = a->kind == PinKind::Output ? a : b;

    if (std::get<NodeInputConnection>(inputPin->connected).id == outputPin->id) {
        return false;
    }

    return true;
}

Node* Editor::SpawnNode(NodeDefinitions::NodeDef* def)
{
    m_Nodes.emplace_back();
    def->CopyToNode(m_NextIdBound, m_Nodes.back());
    m_Nodes.back().Build();

    return &m_Nodes.back();
}

Link* Editor::SpawnLink(Pin* startPin, Pin* endPin)
{
    if (std::get<NodeInputConnection>(endPin->connected).id.Get() != 0) {
        for (auto iter = m_Links.begin(); iter != m_Links.end(); iter++) {
            if (iter->endPinID == endPin->id) {
                DestroyLinkByIter(iter);
                break;
            }
        }
    }

    std::get<NodeOutputConnection>(startPin->connected).ids.emplace_back(endPin->id);
    auto& inputCon = std::get<NodeInputConnection>(endPin->connected);
    inputCon.id = startPin->id;
    inputCon.nodeId = startPin->node;
    inputCon.typeName = startPin->def->typeName;

    m_Links.emplace_back(Link(GetNextId(), startPin->id, endPin->id));
    m_Links.back().color = GetIconColor(startPin->type);
    return &m_Links.back();
}

void Editor::DestroyLink(ed::LinkId id)
{
    for (auto iter = m_Links.begin(); iter != m_Links.end(); iter++) {
        if (iter->id == id) {
            DestroyLinkByIter(iter);
            break;
        }
    }    
}

void Editor::DestroyLinkByIter(std::vector<Link>::iterator& iter)
{
    auto startPin = FindPin(iter->startPinID);
    auto endPin = FindPin(iter->endPinID);
    if (startPin && endPin) {
        auto& outputIds = std::get<NodeOutputConnection>(startPin->connected).ids;
        for (auto id_it = outputIds.begin(); id_it != outputIds.end(); id_it++) {
            if (*id_it == endPin->id) {
                outputIds.erase(id_it);
                break;
            }
        }
        std::get<NodeInputConnection>(endPin->connected).id = 0;
    }
    m_Links.erase(iter);
}

void Editor::DestroyNode(ed::NodeId id)
{
    auto iter = std::find_if(m_Nodes.begin(), m_Nodes.end(), [id](auto& node) { return node.id == id; });
    if (iter != m_Nodes.end())
        m_Nodes.erase(iter);
}

ImColor Editor::GetIconColor(PinType type)
{
    switch (type)
    {
    default:
    case PinType::Flow:     return ImColor(255, 255, 255);
    case PinType::Bool:     return ImColor(220, 48, 48);
    case PinType::Int:      return ImColor(68, 201, 156);
    case PinType::Pose:    return ImColor(147, 226, 74);
    case PinType::String:   return ImColor(124, 21, 153);
    case PinType::Object:   return ImColor(51, 150, 215);
    case PinType::Float: return ImColor(218, 0, 183);
    case PinType::Delegate: return ImColor(255, 48, 48);
    }
};

void Editor::DrawPinIcon(const Pin& pin, bool connected, int alpha)
{
    IconType iconType;
    ImColor  color = GetIconColor(pin.type);
    color.Value.w = alpha / 255.0f;
    switch (pin.type)
    {
    case PinType::Flow:     iconType = IconType::Flow;   break;
    case PinType::Bool:     iconType = IconType::Circle; break;
    case PinType::Int:      iconType = IconType::Circle; break;
    case PinType::Pose:    iconType = IconType::Circle; break;
    case PinType::String:   iconType = IconType::Circle; break;
    case PinType::Object:   iconType = IconType::Circle; break;
    case PinType::Float: iconType = IconType::Circle; break;
    case PinType::Delegate: iconType = IconType::Square; break;
    default:
        return;
    }

    ImGui_Icon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)), iconType, connected, color, ImColor(32, 32, 32, alpha));
}

void Editor::BeginCustomValue(float itemWidth, int id)
{
    ImGui::SameLine();
    ImGui::PushItemWidth(itemWidth);
    ImGui::PushFont(Main::g_mainFontMedium);
    ImGui::PushID(id);
}

void Editor::EndCustomValue()
{
    ImGui::PopID();
    ImGui::PopFont();
    ImGui::PopItemWidth();
}

void Editor::OnFrame(ImGuiIO& io)
{
    ed::SetCurrentEditor(m_Editor);
    auto windowSize = ImGui::GetWindowSize();
    ed::Begin("Editor");

    ed::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 120.0f);
    auto cursorTopLeft = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(cursorTopLeft);

    OnFrame_RenderNodes(io);
    OnFrame_RenderLinks(io);

    OnFrame_UpdatePendingCreations(io);
    OnFrame_UpdatePendingDeletions(io);

    if (!m_CreatingNewNode) {
        m_NewNodePosition = ImGui::GetMousePos();
    }
    
    ed::Suspend();
    OnFrame_RenderNewNodeMenu(io);

    /*
    if (ImGui::BeginPopup("Link Context Menu"))
    {
        auto link = FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link)
        {
            ImGui::Text("ID: %p", link->id.AsPointer());
            ImGui::Text("From: %p", link->startPinID.AsPointer());
            ImGui::Text("To: %p", link->endPinID.AsPointer());
        }
        else
            ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteLink(contextLinkId);
        ImGui::EndPopup();
    }
    */

    ed::Resume();
    ed::PopStyleVar();
    ed::End();
    ed::SetCurrentEditor(nullptr);
}

void Editor::OnFrame_RenderNodes(ImGuiIO& io)
{
    Util::NodeBuilder builder(m_HeaderBackground, 0, 0);

    for (auto& node : m_Nodes)
    {
        builder.Begin(node.id);
        builder.BeginHeader(node.color);
        ImGui::TextUnformatted(node.name.c_str());
        builder.EndHeader();

        builder.BeginLeft();
        for (auto& input : node.inputs)
        {
            auto alpha = ImGui::GetStyle().Alpha;
            if (m_NewLinkPin && !CanCreateLink(m_NewLinkPin, &input) && &input != m_NewLinkPin)
                alpha = alpha * (48.0f / 255.0f);

            builder.BeginInput(input.id);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

            if (input.type < PinType::CustomStart) {
                DrawPinIcon(input, IsPinLinked(input.id), (int)(alpha * 255));
            }
            else {
                ImGui::Dummy(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)));
            }
            
            if (!input.name.empty())
            {
                ImGui::SameLine();
                ImGui::TextUnformatted(input.name.c_str());
            }

            ImGui::PopStyleVar();
            builder.EndInput();

            switch (input.type) {
            case PinType::CustomInt:
                BeginCustomValue(100.0f, input.id.Get());
                ImGui::InputInt("", &std::get<NodeIntCustomValueConnection>(input.connected).value, 1, 5);
                EndCustomValue();
                break;
            case PinType::CustomString:
                BeginCustomValue(200.0f, input.id.Get());
                ImGui::InputText("", &std::get<NodeStringCustomValueConnection>(input.connected).value);
                EndCustomValue();
                break;
            case PinType::CustomFloat:
                BeginCustomValue(130.0f, input.id.Get());
                ImGui::InputFloat("", &std::get<NodeFloatCustomValueConnection>(input.connected).value, 0.1f, 0.5f);
                EndCustomValue();
                break;
            default:
                break;
            }
        }
        builder.EndLeft();

        static float maxSizeX;
        static std::array<float, 100> sizeXBuffer;

        maxSizeX = 0.0f;
        auto currentSizeBuffer = sizeXBuffer.begin();
        for (auto& output : node.outputs) {
            auto& sizeX = *currentSizeBuffer;
            sizeX = ImGui::CalcTextSize(output.name.c_str()).x;
            maxSizeX = std::max(sizeX, maxSizeX);
            ++currentSizeBuffer;
        }

        currentSizeBuffer = sizeXBuffer.begin();
        builder.BeginRight();
        for (auto& output : node.outputs)
        {
            auto& sizeX = *currentSizeBuffer;
            auto alpha = ImGui::GetStyle().Alpha;
            if (m_NewLinkPin && !CanCreateLink(m_NewLinkPin, &output) && &output != m_NewLinkPin)
                alpha = alpha * (48.0f / 255.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            builder.BeginOutput(output.id, std::max(0.0f, maxSizeX - sizeX));

            /*
            if (output.type == PinType::String)
            {
                static char buffer[128] = "Edit Me\nMultiline!";
                static bool wasActive = false;

                ImGui::PushItemWidth(100.0f);
                ImGui::InputText("##edit", buffer, 127);
                ImGui::PopItemWidth();
                if (ImGui::IsItemActive() && !wasActive)
                {
                    ed::EnableShortcuts(false);
                    wasActive = true;
                }
                else if (!ImGui::IsItemActive() && wasActive)
                {
                    ed::EnableShortcuts(true);
                    wasActive = false;
                }
                ImGui::SameLine();
            }
            */
            ImGui::TextUnformatted(output.name.c_str());
            ImGui::SameLine();
            DrawPinIcon(output, IsPinLinked(output.id), (int)(alpha * 255));
            ImGui::PopStyleVar();
            builder.EndOutput();
            ++currentSizeBuffer;
        }
        builder.EndRight();

        builder.End();
    }
}

void Editor::OnFrame_RenderLinks(ImGuiIO& io)
{
    for (auto& link : m_Links)
        ed::Link(link.id, link.startPinID, link.endPinID, link.color, 2.0f);
}

void Editor::OnFrame_UpdatePendingCreations(ImGuiIO& io)
{
    if (m_CreatingNewNode)
        return;

    if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
    {
        const auto showLabel = [](const char* label, ImColor color)
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                auto size = ImGui::CalcTextSize(label);

                auto padding = ImGui::GetStyle().FramePadding;
                auto spacing = ImGui::GetStyle().ItemSpacing;

                ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                auto rectMin = ImGui::GetCursorScreenPos() - padding;
                auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                auto drawList = ImGui::GetWindowDrawList();
                drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                ImGui::TextUnformatted(label);
            };

        ed::PinId startPinId = 0, endPinId = 0;
        if (ed::QueryNewLink(&startPinId, &endPinId))
        {
            auto startPin = FindPin(startPinId);
            auto endPin = FindPin(endPinId);

            //m_NewLinkPin = startPin ? startPin : endPin;

            if (startPin && startPin->kind == PinKind::Input)
            {
                std::swap(startPin, endPin);
                std::swap(startPinId, endPinId);
            }

            if (startPin && endPin)
            {
                if (CanCreateLink(startPin, endPin)) {
                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                    if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                    {
                        SpawnLink(startPin, endPin);
                    }
                }
                else {
                    showLabel("x Invalid Link", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                }
            }
        }

        ed::PinId pinId = 0;
        if (ed::QueryNewNode(&pinId))
        {
            m_NewLinkPin = FindPin(pinId);
            if (m_NewLinkPin)
                showLabel("+ Create Node", ImColor(32, 45, 32, 180));

            if (ed::AcceptNewItem())
            {
                m_CreatingNewNode = true;
                m_NewNodeLinkPin = FindPin(pinId);
                m_NewLinkPin = nullptr;
                ed::Suspend();
                ImGui::OpenPopup("Create New Node");
                ed::Resume();
            }
        }
    }
    else {
        m_NewLinkPin = nullptr;
    }

    ed::EndCreate();
}

void Editor::OnFrame_UpdatePendingDeletions(ImGuiIO& io)
{
    if (m_CreatingNewNode)
        return;

    if (ed::BeginDelete())
    {
        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId))
        {
            if (ed::AcceptDeletedItem())
            {
                DestroyNode(nodeId);
            }
        }

        ed::LinkId linkId = 0;
        while (ed::QueryDeletedLink(&linkId))
        {
            if (ed::AcceptDeletedItem())
            {
                DestroyLink(linkId);
            }
        }
    }
    ed::EndDelete();
}

void Editor::OnFrame_RenderNewNodeMenu(ImGuiIO& io)
{
    if (ImGui::BeginPopup("Create New Node"))
    {
        ImGui::Dummy(ImVec2(0, 8));

        Node* node = nullptr;
        auto& defs = NodeDefinitions::GetDefList();

        for (auto& d : defs) {
            if (ImGui::MenuItem(d->name.data()))
                node = SpawnNode(d);
        }

        ImGui::Dummy(ImVec2(0, 8));

        if (node)
        {
            m_CreatingNewNode = false;
            ed::SetNodePosition(node->id, m_NewNodePosition);

            if (m_NewNodeLinkPin) {
                if (m_NewNodeLinkPin->kind == PinKind::Output) {
                    for (auto& pin : node->inputs) {
                        if (pin.type == m_NewNodeLinkPin->type && CanCreateLink(m_NewNodeLinkPin, &pin)) {
                            SpawnLink(m_NewNodeLinkPin, &pin);
                            break;
                        }
                    }
                }
                else {
                    for (auto& pin : node->outputs) {
                        if (pin.type == m_NewNodeLinkPin->type && CanCreateLink(&pin, m_NewNodeLinkPin)) {
                            SpawnLink(&pin, m_NewNodeLinkPin);
                            break;
                        }
                    }
                }
            }
        }

        ImGui::EndPopup();
    }
    else {
        m_CreatingNewNode = false;
    }
}
