#pragma once
#include "imgui.h"
#include <filesystem>

namespace Main
{
	extern std::filesystem::path pendingOpenFile;

	void OnStart(ImGuiIO& io);
	void OnStop(ImGuiIO& io);
	void OnFrame(ImGuiIO& io);
}