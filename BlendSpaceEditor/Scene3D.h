#pragma once
#include "Renderer.h"
#include <GeometricPrimitive.h>
#include <SimpleMath.h>
#include <numbers>

class Scene3D : public Renderer::RenderTarget
{
public:
	static constexpr DirectX::SimpleMath::Vector3 UP_DIR = { 0.0f, 0.0f, 1.0f };
	static constexpr DirectX::SimpleMath::Vector3 FORWARD_DIR = { 0.0f, 1.0f, 0.0f };
	static constexpr float DEGREE_TO_RAD = { std::numbers::pi / 180.0f };
	static constexpr float RAD_TO_DEGREE = { 180.0f / std::numbers::pi };
	
    class Camera
	{
	public:
		void LookAt(const DirectX::SimpleMath::Vector3& a_pos);
		void SetTranslation(const DirectX::SimpleMath::Vector3& a_pos);
		void OrbitAround(const DirectX::SimpleMath::Vector3& a_pos, float a_relYaw, float a_relPitch);
		void ProcessDebugZoomOrbitControls(float a_deltaTime, float a_orbitSpeed = 45.0f, float a_zoomSpeed = 0.2f, const DirectX::SimpleMath::Vector3& a_orbitPoint = { 0.0f, 0.0f, 0.0f });
		
        DirectX::SimpleMath::Vector3 translation = { 0.0f, 0.0f, 0.0f };
		DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f };
		float fov = 45.0f * DEGREE_TO_RAD;
		bool viewDirty = true;
		bool projDirty = true;
		
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix projection;
    };

    struct Shape
    {
        std::unique_ptr<DirectX::GeometricPrimitive> primitive;
        DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::Identity;
        DirectX::XMVECTOR color = DirectX::Colors::Gray;
        bool wireframe = false;
    };

    Scene3D();

    std::unique_ptr<DirectX::CommonStates> states;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthSV;
    DirectX::SimpleMath::Vector4 clearColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<Shape> shapes;
	Camera cam;

	static std::unique_ptr<DirectX::GeometricPrimitive> CreateBonePrimitive(ID3D11DeviceContext* a_d3dDeviceContext, float a_thickness, float a_length, float a_midpoint);
    virtual bool Init(ID3D11Device* a_d3dDevice,  std::uint32_t a_width, std::uint32_t a_height) override;
	virtual void Render(ID3D11DeviceContext* a_d3dDeviceContext) override;
	void AddDebugShapes(ID3D11DeviceContext* a_d3dDeviceContext);
};