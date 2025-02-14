#pragma once
#include <DirectXMath.h>
#include <array>
#include <cstdint>
#include <memory>
#include <vector>
#include <d3d11.h>
#include <imgui.h>
#include "wrl/client.h"
#include "directxtk/SimpleMath.h"
#include "directxtk/GeometricPrimitive.h"
#include "directxtk/CommonStates.h"

class Renderer
{
public:
	class RenderTarget
	{
	public:
		std::uint32_t width;
		std::uint32_t height;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

		virtual bool Init(ID3D11Device* a_d3dDevice, std::uint32_t a_width, std::uint32_t a_height);
		virtual void Render(ID3D11DeviceContext* a_d3dDeviceContext);
	};
	
    static Renderer& GetSingleton();
    bool Init(const wchar_t* a_windowTitle);
	bool StartFrame();
	void RenderExtraTargets();
    void Present();
    void Shutdown();
	void SetUseVSync(bool a_use);
	RenderTarget* CreateExtraRenderTarget(std::unique_ptr<RenderTarget> a_base, std::uint32_t a_width, std::uint32_t a_height);

	ImVec4 clearColor;
	HWND mainHWND;
	WNDCLASSEXW wc;
	ImGuiIO* io;
	std::uint32_t syncInterval = 1;

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dDeviceContext = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	bool swapChainOccluded = false;
	std::uint32_t resizeWidth = 0;
	std::uint32_t resizeHeight = 0;
	ID3D11RenderTargetView* mainRenderTargetView = nullptr;
	std::vector<std::unique_ptr<RenderTarget>> extraRenderTargets;

private:
	bool CreateDeviceD3D();
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
};