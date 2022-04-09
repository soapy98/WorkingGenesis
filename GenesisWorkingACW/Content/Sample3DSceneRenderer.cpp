#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <atlbase.h>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <Floating.h>
using namespace DirectX;

using namespace GenesisWorkingACW;

using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	//Required
	auto pos = XMFLOAT3(0, 0, 1);
	m_StarrySky = std::make_unique<StarrySky>(deviceResources);
	m_BillBoard = std::make_unique<BillBoard>(deviceResources);
	m_ImplicitRay = std::make_unique<ImplicitRay>(deviceResources);
	m_TessSphere = std::make_unique<TesseltatedSphere>(deviceResources,false);
	m_CamTessSphere = std::make_unique<TesseltatedSphere>(deviceResources,false);
	m_CamTessSphere->SetTessShader(true);
	m_pottery = std::make_unique<AncientPottery>(deviceResources);

	m_ShinyObjs = std::make_unique<ShinyObjs>(deviceResources);

	m_terrain = std::make_unique<Terrain>(deviceResources);
	
	//My own
	m_torus = std::make_unique<Torus>(deviceResources);
	m_sun = std::make_unique<Sun>(deviceResources);
	m_blackHole = std::make_unique<BlackHole>(deviceResources);;
	m_frac = std::make_unique<Effects>(deviceResources, L"fRAC.cso");
	m_stars = std::make_unique<Effects>(deviceResources, L"StarsPS.cso");
//	
	m_explode.emplace_back(std::make_unique<Explode>(deviceResources));
	m_explode.emplace_back(std::make_unique<Explode>(deviceResources));
	m_explode.emplace_back(std::make_unique<Explode>(deviceResources));
m_float.emplace_back(std::make_unique<Floating>(deviceResources));
m_float.emplace_back(std::make_unique<Floating>(deviceResources));
m_float.emplace_back(std::make_unique<Floating>(deviceResources));
	//Positions

	m_torus->SetPos(0, 0.3, 1.5);
	m_CamTessSphere->SetPos(0.5, 2, 1.5);
	m_TessSphere->SetPos(-0.5, 2, 1.5);
	float x = -0.5, y = 2, z = 1.2;
	float rotX = 0.0f, rotY = 90.0f, rotZ = -10.f;
	for (auto& floa:m_float)
	{
		floa->SetScale(1, 1.5, 1);
		floa->SetPos(x , y, z);
		floa->SetRot(rotX, rotY, rotZ);
		rotY += 25;
			rotZ+=15;
		/*rotX /= 2;
		rotY += 25;*/
	/*	rotX += 10;*/
		//rotY += 7;
		x += 2;
	}
	m_sun->SetPos(0, 0, -600);
	m_pottery->SetPos(0, 1, 3);
	m_ShinyObjs->SetPos(1, 2.5, -3);

	m_terrain->SetPos(0, -1, 0);
	m_blackHole->SetPos(0, 0, 700);
	float startPosX = 2;
	for (auto& star :m_explode)
	{
	star->SetPos(startPosX, 10, 0);
	startPosX += 4;
	}

	
	m_cam = std::make_unique<Camera>();
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
	);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	/*XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);*/
	m_cam->SetProjectionValues(XM_PIDIV2, aspectRatio, 0.01f, 100.0f);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(m_cam->GetProjectionMatrix())
	);

	auto lookat = XMFLOAT3(0, 0.1, 0.1);
	m_cam->SetLookAtPos(lookat);
	auto pos = XMFLOAT3(0.0f, 0.5f, -0.5f);
	m_cam->SetPosition(XMLoadFloat3(&pos));

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(m_cam->GetViewMatrix()));





}
void Sample3DSceneRenderer::CheckInputCameraMovement(DX::StepTimer const& timer)
{

	if ((QueryKeyPressed(VirtualKey::Left) || QueryKeyPressed(VirtualKey::A)) && !QueryKeyPressed(VirtualKey::Control))
	{
		m_cam->addYaw(-3 * timer.GetElapsedSeconds());
	}

	//Camera Rotate Right
	if ((QueryKeyPressed(VirtualKey::Right) || QueryKeyPressed(VirtualKey::D)) && !QueryKeyPressed(VirtualKey::Control))
	{
		m_cam->addYaw(3 * timer.GetElapsedSeconds());
	}
	//Camera Rotate Up
	if ((QueryKeyPressed(VirtualKey::Up) || QueryKeyPressed(VirtualKey::W)) && !QueryKeyPressed(VirtualKey::Control))
	{
		m_cam->addPitch(-3 * timer.GetElapsedSeconds());
	}
	//Camera Rotate Down
	if ((QueryKeyPressed(VirtualKey::Down) || QueryKeyPressed(VirtualKey::S)) && !QueryKeyPressed(VirtualKey::Control))
	{
		m_cam->addPitch(3 * timer.GetElapsedSeconds());
	}

	//PAN directional controls
				//Pan Left
	if (QueryKeyPressed(VirtualKey::Control) && QueryKeyPressed(VirtualKey::Left))
	{
		m_cam->AdjustPosition(-3 * timer.GetElapsedSeconds(), 0, 0);
	}
	//Pan Right
	if (QueryKeyPressed(VirtualKey::Control) && QueryKeyPressed(VirtualKey::Right))
	{
		m_cam->AdjustPosition(3 * timer.GetElapsedSeconds(), 0, 0);
	}
	//Pan forward
	if (QueryKeyPressed(VirtualKey::Control) && QueryKeyPressed(VirtualKey::Up))
	{
		m_cam->AdjustPosition(0, 0, 3 * timer.GetElapsedSeconds());
	}
	//Pan Backward
	if (QueryKeyPressed(VirtualKey::Control) && QueryKeyPressed(VirtualKey::Down))
	{
		m_cam->AdjustPosition(0, 0, -3 * timer.GetElapsedSeconds());
	}
	//Pan Down
	if (QueryKeyPressed(VirtualKey::Control) && QueryKeyPressed(VirtualKey::PageDown))
	{
		m_cam->AdjustPosition(0, 3 * timer.GetElapsedSeconds(), 0);
	}
	//Pan Up
	if (QueryKeyPressed(VirtualKey::Control) && QueryKeyPressed(VirtualKey::PageUp))
	{
		m_cam->AdjustPosition(0, -3 * timer.GetElapsedSeconds(), 0);
	}
	if (QueryKeyPressed(VirtualKey::R))
	{
		auto pos = XMFLOAT3(0, 0, -5);
		m_cam->SetPosition(XMLoadFloat3(&pos));
	}

}

bool Sample3DSceneRenderer::QueryKeyPressed(VirtualKey key)
{
	return (CoreWindow::GetForCurrentThread()->GetKeyState(key) & CoreVirtualKeyStates::Down) == CoreVirtualKeyStates::Down;
}
// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	CheckInputCameraMovement(timer);
	m_StarrySky->Update(timer);
	m_pottery->Update(timer);
	m_ShinyObjs->Update(timer);
	m_ImplicitRay->Update(timer);
	m_BillBoard->Update(timer);
	m_torus->Update(timer);
	m_TessSphere->Update(timer);
	m_terrain->Update(timer);
	for (auto& floa : m_float)
	{
		floa->Update(timer);
	}
	m_CamTessSphere->Update(timer);
	for (auto& star : m_explode)
	{
		star->Update(timer);
	}
	m_sun->Update(timer);
	m_blackHole->Update(timer);
	m_frac->Update(timer);
	m_stars->Update(timer);
}





// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	m_cam->UpdateViewMatrix();
	auto cam = m_cam->GetPositionFloat3();
	auto world = m_cam->GetViewMatrix();
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(m_cam->GetViewMatrix()));
	////auto world = XMMatrixIdentity();
	////Required
	m_stars->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_stars->SetCameraPositionConstantBuffer(cam);
	m_stars->Render();
		m_sun->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_sun->SetCameraPositionConstantBuffer(cam);
	m_sun->Render();
	m_blackHole->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_blackHole->SetCameraPositionConstantBuffer(cam);
	m_blackHole->Render();

	
	m_terrain->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_terrain->SetCameraPositionConstantBuffer(cam);
	m_terrain->Render();
	m_BillBoard->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_BillBoard->SetCameraPositionConstantBuffer(cam);
	m_BillBoard->Render();
	m_TessSphere->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_TessSphere->SetCameraPositionConstantBuffer(cam);
	m_TessSphere->Render();
	m_CamTessSphere->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_CamTessSphere->SetCameraPositionConstantBuffer(cam);
	m_CamTessSphere->Render();
	m_pottery->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_pottery->SetCameraPositionConstantBuffer(cam);
	m_pottery->Render();
	for (auto& floa : m_float)
	{
		floa->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
		floa->SetCameraPositionConstantBuffer(cam);
		floa->Render();
	}

//


	

m_ShinyObjs->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_ShinyObjs->SetCameraPositionConstantBuffer(cam);
	m_ShinyObjs->Render();

	m_StarrySky->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_StarrySky->SetCameraPositionConstantBuffer(cam);
	m_StarrySky->Render();
	
	

	
//Implicit effects	

auto context = m_deviceResources->GetD3DDeviceContext(); context->RSSetState(m_rast.Get());
	

	// 
//	//my Own
m_TessSphere->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_TessSphere->SetCameraPositionConstantBuffer(cam);
	m_TessSphere->Render();
	m_CamTessSphere->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_CamTessSphere->SetCameraPositionConstantBuffer(cam);
	m_CamTessSphere->Render();
	m_pottery->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_pottery->SetCameraPositionConstantBuffer(cam);
	m_pottery->Render();
	for (auto& floa : m_float)
	{
		floa->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
		floa->SetCameraPositionConstantBuffer(cam);
		floa->Render();
	}
	m_terrain->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_terrain->SetCameraPositionConstantBuffer(cam);
	m_terrain->Render();
//

for (auto& star : m_explode)
{
	star->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	star->SetCameraPositionConstantBuffer(cam);
	star->Render();
}


	 // Loading is asynchronous. Only draw geometry after it's loaded.
	



	m_torus->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_torus->SetCameraPositionConstantBuffer(cam);
	m_torus->Render();
	//m_frac->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	//m_frac->SetCameraPositionConstantBuffer(cam);
	//m_frac->Render();
		m_ImplicitRay->SetViewProjectionMatrixConstantBuffer(XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.view)), XMMatrixTranspose(XMLoadFloat4x4(&m_constantBufferData.projection)));
	m_ImplicitRay->SetCameraPositionConstantBuffer(cam);
	m_ImplicitRay->Render();

}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	CD3D11_RASTERIZER_DESC rasterStateDesc(D3D11_DEFAULT);

	rasterStateDesc.CullMode = D3D11_CULL_NONE;
	rasterStateDesc.FillMode = D3D11_FILL_SOLID;

	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateRasterizerState(
			&rasterStateDesc,
			m_rast.GetAddressOf()
		)
	);
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_ImplicitRay->ReleaseDeviceDependentResources();
	m_StarrySky->ReleaseDeviceDependentResources();
	m_BillBoard->ReleaseDeviceDependentResources();
	m_torus->ReleaseDeviceDependentResources();
	m_ShinyObjs->ReleaseDeviceDependentResources();
	m_pottery->ReleaseDeviceDependentResources();
	m_TessSphere->ReleaseDeviceDependentResources();
	m_CamTessSphere->ReleaseDeviceDependentResources();
	m_terrain->ReleaseDeviceDependentResources();
	for (auto& floa : m_float)
	{
		floa->ReleaseDeviceDependentResources();
	}
	for (auto& star : m_explode)
	{
		star->ReleaseDeviceDependentResources();
	}
	m_blackHole->ReleaseDeviceDependentResources();
	m_frac->ReleaseDeviceDependentResources();
	m_sun->ReleaseDeviceDependentResources();
	m_stars->ReleaseDeviceDependentResources();
}