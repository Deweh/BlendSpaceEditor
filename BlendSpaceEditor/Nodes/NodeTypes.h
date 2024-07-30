#pragma once
#include <imgui-node-editor/imgui_node_editor.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <memory>
#include <variant>

namespace NodeDefinitions
{
    struct PinDef;
    struct NodeDef;
}

namespace ed = ax::NodeEditor;

enum class PinType : uint16_t
{
    Flow = 0,
    Bool = 1,
    Int = 2,
    Pose = 3,
    String = 4,
    Object = 5,
    Float = 6,
    Delegate = 7,
    CustomStart = 8,
    CustomInt = 9,
    CustomString = 10,
    CustomFloat = 11
};

enum class PinKind
{
    Output,
    Input
};

struct Node;
struct Link;

struct NodeInputConnection
{
    ed::PinId id{ 0 };
    ed::NodeId nodeId{ 0 };
    std::string typeName;
};

struct NodeOutputConnection
{
    std::vector<ed::PinId> ids;
};

struct NodeStringCustomValueConnection
{
    std::string value{ "" };
};

struct NodeIntCustomValueConnection
{
    int value{ 0 };
};

struct NodeFloatCustomValueConnection
{
    float value{ .0f };
};

struct Pin
{
    using ConnectionVariant = std::variant<
        NodeInputConnection,
        NodeOutputConnection,
        NodeStringCustomValueConnection,
        NodeIntCustomValueConnection,
        NodeFloatCustomValueConnection>;

    ed::PinId id;
    ed::NodeId node;
    std::string name;
    PinType type;
    PinKind kind;
    ConnectionVariant connected;
    NodeDefinitions::PinDef* def;

    Pin(int _id, const char* _name, PinType _type) :
        id(_id), node(0), name(_name), type(_type), kind(PinKind::Input)
    {
    }
};

struct Node
{
    ed::NodeId id;
    std::string name;
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
    ImColor color;
    NodeDefinitions::NodeDef* def;

    inline void Build()
    {
        for (auto& input : inputs)
        {
            input.node = id;
            input.kind = PinKind::Input;
            if (input.type > PinType::CustomStart) {
                switch (input.type) {
                case PinType::CustomInt: input.connected.emplace<NodeIntCustomValueConnection>(); break;
                case PinType::CustomFloat: input.connected.emplace<NodeFloatCustomValueConnection>(); break;
                case PinType::CustomString: input.connected.emplace<NodeStringCustomValueConnection>(); break;
                }
            }
            else {
                input.connected.emplace<NodeInputConnection>();
            }
        }

        for (auto& output : outputs)
        {
            output.node = id;
            output.kind = PinKind::Output;
            output.connected.emplace<NodeOutputConnection>();
        }
    }

    void ToJson(nlohmann::json& obj);
    static void CompactJsonIds(nlohmann::json& arr);
    bool FromJson(nlohmann::json& obj, size_t& maxId);
};

struct Link
{
    ed::LinkId id;
    ed::PinId startPinID;
    ed::PinId endPinID;
    ImColor color;

    Link(ed::LinkId _id, ed::PinId _startPinId, ed::PinId _endPinId) :
        id(_id), startPinID(_startPinId), endPinID(_endPinId), color(255, 255, 255)
    {
    }
};

struct NodeIdLess
{
    inline bool operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
    {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};