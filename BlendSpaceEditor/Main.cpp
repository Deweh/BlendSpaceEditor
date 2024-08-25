#include "Main.h"
#include "Editor.h"
#include <memory>
#include "Win32Util.h"
#include <fstream>

namespace ed = ax::NodeEditor;

extern HWND g_MainHWND;

namespace Main
{
	std::unique_ptr<Editor> g_mainEditor{ nullptr };
    ImFont* g_mainFont{ nullptr };
    ImFont* g_mainFontSmall{ nullptr };
    ImFont* g_mainFontMedium{ nullptr };
    std::string g_statusText{ "" };
    std::filesystem::path g_curPath{ L"" };
    std::filesystem::path pendingOpenFile{ L"" };

	void OnStart(ImGuiIO& io)
	{
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
        io.IniFilename = NULL;
        ImGui::StyleColorsDark();
        g_mainFont = ImGui_LoadWindowsFont("Arial", 22.0f, io);
        g_mainFontSmall = ImGui_LoadWindowsFont("Arial", 16.0f, io);
        g_mainFontMedium = ImGui_LoadWindowsFont("Arial", 18.5f, io);
        g_mainEditor = std::make_unique<Editor>();
	}

	void OnStop(ImGuiIO& io)
	{
        g_mainEditor.reset();
	}

    std::string GetCurrentClockTime() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        std::ostringstream oss;
        tm curLocalTime;
        localtime_s(&curLocalTime, &time);
        oss << std::put_time(& curLocalTime, "%I:%M:%S %p");
        return oss.str();
    }

    void SaveData(const std::filesystem::path& filePath)
    {
        ed::SetCurrentEditor(g_mainEditor->m_Editor);
        nlohmann::json obj;
        obj["version"] = 1;

        auto& nodes = obj["nodes"];
        for (auto& node : g_mainEditor->m_Nodes) {
            node.ToJson(nodes.emplace_back());
        }
        Node::CompactJsonIds(nodes);

        std::ofstream outFile{ filePath };
        if (!outFile.is_open() || !outFile.good()) {
            MessageBoxA(g_MainHWND, "Failed to save file.", "Error", 0);
            return;
        }
        outFile << obj.dump();
        ed::SetCurrentEditor(nullptr);

        g_statusText = std::format("Saved {} at {}", filePath.generic_string(), GetCurrentClockTime());
    }

    void LoadData(const std::filesystem::path& filePath)
    {
        std::ifstream inFile{ filePath };
        if (!inFile.is_open() || !inFile.good()) {
            MessageBoxA(g_MainHWND, "Failed to open file.", "Error", 0);
            return;
        }

        nlohmann::json obj;
        try {
            obj = nlohmann::json::parse(inFile);
        }
        catch (const std::exception& ex) {
            MessageBoxA(g_MainHWND, std::format("Failed to parse blend graph file. Error: {}", ex.what()).c_str(), "Error", 0);
            return;
        }

        g_mainEditor->m_Nodes.clear();
        g_mainEditor->m_Links.clear();

        bool successful = true;
        ed::SetCurrentEditor(g_mainEditor->m_Editor);
        try {
            auto& nodes = obj["nodes"];
            size_t lastId = 0;
            for (auto& n : nodes) {
                if (!n.is_object())
                    continue;

                auto& curNode = g_mainEditor->m_Nodes.emplace_back();
                if (!curNode.FromJson(n, lastId)) {
                    throw std::exception{ "Failed to parse node. " };
                }
            }

            for (auto& n : g_mainEditor->m_Nodes) {
                for (auto& i : n.inputs) {
                    i.id = ++lastId;
                }
                for (auto& o : n.outputs) {
                    o.id = ++lastId;
                }
            }

            if (lastId > INT32_MAX) {
                throw std::exception{ "[P] Node ID exceeds maximum value." };
            }

            g_mainEditor->m_LastId = lastId;

            for (auto& n : g_mainEditor->m_Nodes) {
                for (auto& i : n.inputs) {
                    if (i.type < PinType::CustomStart)
                    {
                        auto& connected = std::get<NodeInputConnection>(i.connected);
                        auto targetNode = g_mainEditor->FindNode(connected.nodeId);
                        if (!targetNode)
                            continue;

                        for (auto& o : targetNode->outputs) {
                            if (o.def->typeName == connected.typeName) {
                                connected.id = o.id;
                                auto& outConnect = std::get<NodeOutputConnection>(o.connected);
                                outConnect.ids.push_back(i.id);
                                g_mainEditor->m_Links.emplace_back(g_mainEditor->GetNextId(), o.id, i.id);
                                g_mainEditor->m_Links.back().color = g_mainEditor->GetIconColor(i.type);
                                break;
                            }
                        }
                    }
                }
            }
        }
        catch (const std::exception& ex) {
            successful = false;
            g_mainEditor->m_Nodes.clear();
            g_mainEditor->m_Links.clear();
            MessageBoxA(g_MainHWND, std::format("Failed to load blend graph file. Error: {}", ex.what()).c_str(), "Error", 0);
        }
        
        ed::SetCurrentEditor(nullptr);

        if (!successful) {
            g_mainEditor->InitNew();
            g_curPath = L"";
            g_statusText = "";
        }
        else {
            g_statusText = std::format("Loaded {} at {}", filePath.generic_string(), GetCurrentClockTime());
        }
    }

    void OnLoad()
    {
        auto result = Win32Util_OpenFileDialog(false, g_MainHWND, L"Blend Tree Files (*.bt)\0*.bt\0");
        if (!result.empty()) {
            g_curPath = result;
        }
        else {
            return;
        }
        LoadData(g_curPath);
    }

    void OnSave(bool forceChoosePath)
    {
        if (g_curPath.empty() || forceChoosePath) {
            auto result = Win32Util_OpenFileDialog(true, g_MainHWND, L"Blend Tree Files (*.bt)\0*.bt\0");
            if (!result.empty()) {
                g_curPath = result;
                g_curPath = g_curPath.replace_extension(".bt");
            }
            else {
                return;
            }
        }
        SaveData(g_curPath);
    }

	void OnFrame(ImGuiIO& io)
	{
        ImGui::PushFont(g_mainFontSmall);
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::SetNextWindowPos({ .0f, .0f });
		ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);

        if (!pendingOpenFile.empty()) {
            g_curPath = pendingOpenFile;
            pendingOpenFile.clear();
            LoadData(g_curPath);
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
            if (ImGui::IsKeyReleased(ImGuiKey_S, false)) {
                OnSave(false);
            }
            else if (ImGui::IsKeyReleased(ImGuiKey_O, false)) {
                OnLoad();
            }
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New")) {
                    g_mainEditor->InitNew();
                    g_curPath = L"";
                    g_statusText = "";
                }
                if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                    OnLoad();
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    OnSave(false);
                }
                if (ImGui::MenuItem("Save As...", "Ctrl+S")) {
                    OnSave(true);
                }
                ImGui::EndMenu();
            }

            ImGui::SameLine((ImGui::GetWindowWidth() - ImGui::CalcTextSize(g_statusText.c_str()).x) - 20);
            ImGui::TextUnformatted(g_statusText.c_str());

            ImGui::EndMenuBar();
        }
        
		//ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
        ImGui::PopFont();
        ImGui::PushFont(g_mainFont);
		//ImGui::Separator();
        g_mainEditor->OnFrame(io);
        ImGui::PopFont();
        ImGui::End();
        
	}
}