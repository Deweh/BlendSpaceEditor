#pragma once
#include "ImUtil.h"
#include "imgui-node-editor/imgui_node_editor.h"
#include "Nodes/NodeTypes.h"
#include "Nodes/NodeDefinitions.h"
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <functional>

namespace ed = ax::NodeEditor;

class Editor
{
public:
	Editor();
	~Editor();

    void InitNew();
    int GetNextId();
    Node* FindNode(ed::NodeId id);
    Link* FindLink(ed::LinkId id);
    Pin* FindPin(ed::PinId id);
    bool IsPinLinked(ed::PinId id);
    bool CanCreateLink(Pin* a, Pin* b);
    Node* SpawnNode(NodeDefinitions::NodeDef* def);
    Link* SpawnLink(Pin* startPin, Pin* endPin);
    void DestroyLink(ed::LinkId id);
    void DestroyLinkByIter(std::vector<Link>::iterator& iter);
    void DestroyNode(ed::NodeId id);
    ImColor GetIconColor(PinType type);
    void DrawPinIcon(const Pin& pin, bool connected, int alpha);
    void BeginCustomValue(float itemWidth, int id);
    void EndCustomValue();

    void OnFrame(ImGuiIO& io);
    void OnFrame_RenderNodes(ImGuiIO& io);
    void OnFrame_RenderLinks(ImGuiIO& io);
    void OnFrame_UpdatePendingCreations(ImGuiIO& io);
    void OnFrame_UpdatePendingDeletions(ImGuiIO& io);
    void OnFrame_RenderNewNodeMenu(ImGuiIO& io);

    //Render Context
    bool m_CreatingNewNode = false;
    ed::NodeId m_ContextNodeId = 0;
    ed::LinkId m_ContextLinkId = 0;
    ed::PinId m_ContextPinId = 0;
    Pin* m_NewNodeLinkPin = nullptr;
    Pin* m_NewLinkPin = nullptr;
    ImVec2 m_NewNodePosition = { 0.0f, 0.0f };

    //Members
	ed::EditorContext* m_Editor = nullptr;
    int m_LastId = 0;
    const int m_PinIconSize = 24;
    const std::function<int()> m_NextIdBound = std::bind(&Editor::GetNextId, this);
    std::vector<Node> m_Nodes;
    std::vector<Link> m_Links;
    ImTextureID m_HeaderBackground = nullptr;
    ImTextureID m_SaveIcon = nullptr;
    ImTextureID m_RestoreIcon = nullptr;
};