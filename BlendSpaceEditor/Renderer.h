#pragma once
#include <cstdint>
#include <d3d11.h>
#include <imgui.h>

class Renderer
{
public:
	static Renderer& GetSingleton();
	bool Init(const wchar_t* a_windowTitle);
	bool StartFrame();
	void Present();
	void Shutdown();

	ImVec4 clearColor;
	HWND mainHWND;
	WNDCLASSEXW wc;
	ImGuiIO* io;
	
	ID3D11Device* d3dDevice = nullptr;
    ID3D11DeviceContext* d3dDeviceContext = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    bool swapChainOccluded = false;
	std::uint32_t resizeWidth = 0;
	std::uint32_t resizeHeight = 0;
	ID3D11RenderTargetView* mainRenderTargetView = nullptr;

private:
	bool CreateDeviceD3D();
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
};