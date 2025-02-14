#include "Scene3D.h"
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <GeometricPrimitive.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

void Scene3D::Camera::LookAt(const Vector3& a_pos)
{
	const Matrix lookatView = Matrix::CreateLookAt(translation, a_pos, UP_DIR);
	const Matrix lookAtWorld = lookatView.Invert();
	rotation = lookAtWorld.ToEuler();
	viewDirty = true;
}

void Scene3D::Camera::SetTranslation(const Vector3& a_pos)
{
	translation = a_pos;
	viewDirty = true;
}

void Scene3D::Camera::OrbitAround(const Vector3& a_pos, float a_relYaw, float a_relPitch)
{
	const Vector3 offset = translation - a_pos;
	const float radius = offset.Length();
	float currentYaw = atan2f(offset.x, offset.y);
	float currentPitch = asinf(offset.z / radius);
	
    float newYaw = currentYaw + a_relYaw * DEGREE_TO_RAD;
    float newPitch = currentPitch + a_relPitch * DEGREE_TO_RAD;

    const float maxPitch = DirectX::XM_PIDIV2 - 0.001f;
    const float minPitch = -maxPitch;
    if(newPitch > maxPitch) newPitch = maxPitch;
    if(newPitch < minPitch) newPitch = minPitch;
    
    float cosPitch = cosf(newPitch);
    Vector3 newOffset;
    newOffset.x = radius * cosPitch * sinf(newYaw);
    newOffset.y = radius * cosPitch * cosf(newYaw);
    newOffset.z = radius * sinf(newPitch);

	translation = a_pos + newOffset;
	viewDirty = true;
}

void Scene3D::Camera::ProcessDebugZoomOrbitControls(float a_deltaTime, float a_orbitSpeed, float a_zoomSpeed, const Vector3& a_orbitPoint)
{	
	if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
        OrbitAround(a_orbitPoint, a_orbitSpeed * a_deltaTime, 0.0f);
        LookAt(a_orbitPoint);
    }

    if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
        OrbitAround(a_orbitPoint, -a_orbitSpeed * a_deltaTime, 0.0f);
        LookAt(a_orbitPoint);
    }

    if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
        OrbitAround(a_orbitPoint, 0.0f, a_orbitSpeed * a_deltaTime);
        LookAt(a_orbitPoint);
    }

    if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
        OrbitAround(a_orbitPoint, 0.0f, -a_orbitSpeed * a_deltaTime);
        LookAt(a_orbitPoint);
	}

	if (ImGui::IsKeyDown(ImGuiKey_W)) {
		Vector3 direction = (a_orbitPoint - translation);
		direction.Normalize();
		SetTranslation(translation + direction * a_zoomSpeed);
	}

	if (ImGui::IsKeyDown(ImGuiKey_S)) {
		Vector3 direction = (a_orbitPoint - translation);
		direction.Normalize();
		SetTranslation(translation + direction * -a_zoomSpeed);
    }
}

Scene3D::Scene3D()
{
}

bool Scene3D::Init(ID3D11Device* a_d3dDevice, std::uint32_t a_width, std::uint32_t a_height)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = a_width;
	texDesc.Height = a_height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 4;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = a_d3dDevice->CreateTexture2D(&texDesc, nullptr, &texture);
	if (FAILED(hr)) {
		return false;
	}

    hr = a_d3dDevice->CreateRenderTargetView(texture.Get(), nullptr, &rtv);
	if (FAILED(hr)) {
		return false;
    }

	hr = a_d3dDevice->CreateShaderResourceView(texture.Get(), nullptr, &srv);
	if (FAILED(hr)) {
		return false;
	}

	width = a_width;
	height = a_height;

	states = std::make_unique<DirectX::CommonStates>(a_d3dDevice);

	D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 4;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = a_d3dDevice->CreateTexture2D(&depthDesc, nullptr, depthTexture.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	hr = a_d3dDevice->CreateDepthStencilView(depthTexture.Get(), nullptr, depthSV.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}
	
	return true;
}

void Scene3D::Render(ID3D11DeviceContext* a_d3dDeviceContext)
{
	if (cam.viewDirty) {
		DirectX::SimpleMath::Matrix camWorld = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(cam.rotation.y, cam.rotation.x, cam.rotation.z) *
		           DirectX::SimpleMath::Matrix::CreateTranslation(cam.translation);
		cam.view = camWorld.Invert();
		cam.viewDirty = false;
	}

	if (cam.projDirty) {
		cam.projection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(cam.fov, float(width) / float(height), 0.1f, 100.0f);
		cam.projDirty = false;
    }
	
	a_d3dDeviceContext->OMSetRenderTargets(1, rtv.GetAddressOf(), depthSV.Get());
	a_d3dDeviceContext->ClearRenderTargetView(rtv.Get(), &clearColor.x);
	a_d3dDeviceContext->ClearDepthStencilView(depthSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width    = width;
    vp.Height   = height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
	a_d3dDeviceContext->RSSetViewports(1, &vp);

    a_d3dDeviceContext->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
    a_d3dDeviceContext->OMSetDepthStencilState(states->DepthDefault(), 0);
	a_d3dDeviceContext->RSSetState(states->CullCounterClockwise());
	
	for (auto& s : shapes) {
		s.primitive->Draw(s.transform, cam.view, cam.projection, s.color, nullptr, s.wireframe);
	}
}

std::unique_ptr<DirectX::GeometricPrimitive> Scene3D::CreateBonePrimitive(ID3D11DeviceContext* a_d3dDeviceContext, float a_thickness, float a_length, float a_midpoint)
{
	std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<uint16_t> indices;

    const DirectX::XMFLOAT3 equator[] = {
        { a_thickness, 0.0f, a_midpoint},
        { 0.0f, a_thickness, a_midpoint},
        {-a_thickness, 0.0f, a_midpoint},
        { 0.0f, -a_thickness, a_midpoint},
    };

    vertices.emplace_back(DirectX::VertexPositionNormalTexture{ {0.0f, 0.0f, a_length}, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } });

	for (const auto& pos : equator) {
		float length = std::sqrt(pos.x * pos.x + pos.y * pos.y);
        vertices.emplace_back(DirectX::VertexPositionNormalTexture{ pos, { pos.x / length, pos.y / length, 0.0f  }, { 0.0f, 0.0f } });
    }
    
    vertices.emplace_back(DirectX::VertexPositionNormalTexture{ {0.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } });

    const uint16_t indicesArray[] = {
        2,1,0,  3,2,0,  4,3,0,  1,4,0,
        1,2,5,  2,3,5,  3,4,5,  4,1,5
    };
    indices.assign(std::begin(indicesArray), std::end(indicesArray));

	return DirectX::GeometricPrimitive::CreateCustom(
		a_d3dDeviceContext,
		vertices,
		indices);
}

void Scene3D::AddDebugShapes(ID3D11DeviceContext* a_d3dDeviceContext)
{
	auto& s1 = shapes.emplace_back();
    s1.primitive = Scene3D::CreateBonePrimitive(a_d3dDeviceContext, 0.4f, 2.0f, 0.5f);

    auto& s2 = shapes.emplace_back();
    s2.primitive = DirectX::GeometricPrimitive::CreateBox(a_d3dDeviceContext, { 10.0f, 10.0f, 1.0f });
    s2.transform.Translation({ 0.0f, 0.0f, -0.5f });

    auto& s3 = shapes.emplace_back();
    s3.primitive = DirectX::GeometricPrimitive::CreateSphere(a_d3dDeviceContext, 0.15f, 5);

    auto& s4 = shapes.emplace_back();
    s4.primitive = DirectX::GeometricPrimitive::CreateSphere(a_d3dDeviceContext, 0.15f, 5);
    s4.transform.Translation({ 0.0f, 0.0f, 2.0f });
}