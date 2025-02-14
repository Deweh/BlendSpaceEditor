#include "imgui.h"
#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN
#endif
#include "BlendSpaceEditor/Main.h"
#include "BlendSpaceEditor/Renderer.h"
#include <shellapi.h>
#include <tchar.h>
#include <windows.h>


// Main code
int main(int argc, char** argv)
{
	if (argc > 1) {
		Main::pendingOpenFile = argv[1];
	}

	Renderer& renderer = Renderer::GetSingleton();
	if (!renderer.Init(L"BlendGraphEditor")) {
		return 1;
	}
	ImGuiIO& io = ImGui::GetIO();
	Main::OnStart(io);

	// Main loop
	bool done = false;
	while (!done) {
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		if (!renderer.StartFrame()) {
			continue;
		}

		Main::OnFrame(io);
		renderer.Present();
	}

	Main::OnStop(io);
	renderer.Shutdown();
	return 0;
}