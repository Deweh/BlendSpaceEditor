#pragma once
#include "imgui.h"

namespace Main
{
	void OnStart(ImGuiIO& io);
	void OnStop(ImGuiIO& io);
	void OnFrame(ImGuiIO& io);
}