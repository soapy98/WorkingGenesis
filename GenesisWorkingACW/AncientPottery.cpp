#include "pch.h"
#include "AncientPottery.h"
#include <Common/DirectXHelper.h>
using namespace GenesisWorkingACW;
using namespace DirectX;

AncientPottery::AncientPottery(const std::shared_ptr<DX::DeviceResources>& deviceResources) :m_deviceResources(deviceResources), m_position(0.f, 0.f, 5.0f), m_rotation(-1.57f, 0.0f, 0.0f), m_scale(0.5f, .5f, .5f), m_loadingComplete(false), m_indexCount(0)
{
	CreateDeviceDependentResources();
}

void AncientPottery::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"PotteryVS.cso");
	auto loadHSTask = DX::ReadDataAsync(L"PotteryHS.cso");
	auto loadDSTask = DX::ReadDataAsync(L"PotteryDS.cso");
	auto loadPSTask = DX::ReadDataAsync(L"PotteryPS.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
			)
		);
		});

	//// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_MVPBuffer
			)
		);
		CD3D11_BUFFER_DESC camconstantBufferDesc(sizeof(CameraConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&camconstantBufferDesc,
				nullptr,
				&m_cameraBuffer
			)
		);
		});
	auto createHSTask = loadHSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateHullShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_hullShader
			)
		);
		});
	auto createDSTask = loadDSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_domainShader
			)
		);
		});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask && createHSTask && createDSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPosition cubeVertices[] =
		{
			// rim: [0,192]
		{XMFLOAT3(0,-1.4,2.4)},
		{XMFLOAT3(0.784,-1.4,2.4)},
		{XMFLOAT3(1.4,-0.784,2.4)},
		{XMFLOAT3(1.4,0,2.4)},
		{XMFLOAT3(0,-1.3375,2.53125)},
		{XMFLOAT3(0.749,-1.3375,2.53125)},
		{XMFLOAT3(1.3375,-0.749,2.53125)},
		{XMFLOAT3(1.3375,0,2.53125)},
		{XMFLOAT3(0,-1.4375,2.53125)},
		{XMFLOAT3(0.805,-1.4375,2.53125)},
		{XMFLOAT3(1.4375,-0.805,2.53125)},
		{XMFLOAT3(1.4375,0,2.53125)},
		{XMFLOAT3(0,-1.5,2.4)},
		{XMFLOAT3(0.84,-1.5,2.4)},
		{XMFLOAT3(1.5,-0.84,2.4)},
		{XMFLOAT3(1.5,0,2.4)},
		{XMFLOAT3(-1.4,0,2.4)},
		{XMFLOAT3(-1.4,-0.784,2.4)},
		{XMFLOAT3(-0.784,-1.4,2.4)},
		{XMFLOAT3(0,-1.4,2.4)},
		{XMFLOAT3(-1.3375,0,2.53125)},
		{XMFLOAT3(-1.3375,-0.749,2.53125)},
		{XMFLOAT3(-0.749,-1.3375,2.53125)},
		{XMFLOAT3(0,-1.3375,2.53125)},
		{XMFLOAT3(-1.4375,0,2.53125)},
		{XMFLOAT3(-1.4375,-0.805,2.53125)},
		{XMFLOAT3(-0.805,-1.4375,2.53125)},
		{XMFLOAT3(0,-1.4375,2.53125)},
		{XMFLOAT3(-1.5,0,2.4)},
		{XMFLOAT3(-1.5,-0.84,2.4)},
		{XMFLOAT3(-0.84,-1.5,2.4)},
		{XMFLOAT3(0,-1.5,2.4)},
		{XMFLOAT3(1.4,0,2.4)},
		{XMFLOAT3(1.4,0.784,2.4)},
		{XMFLOAT3(0.784,1.4,2.4)},
		{XMFLOAT3(0,1.4,2.4)},
		{XMFLOAT3(1.3375,0,2.53125)},
		{XMFLOAT3(1.3375,0.749,2.53125)},
		{XMFLOAT3(0.749,1.3375,2.53125)},
		{XMFLOAT3(0,1.3375,2.53125)},
		{XMFLOAT3(1.4375,0,2.53125)},
		{XMFLOAT3(1.4375,0.805,2.53125)},
		{XMFLOAT3(0.805,1.4375,2.53125)},
		{XMFLOAT3(0,1.4375,2.53125)},
		{XMFLOAT3(1.5,0,2.4)},
		{XMFLOAT3(1.5,0.84,2.4)},
		{XMFLOAT3(0.84,1.5,2.4)},
		{XMFLOAT3(0,1.5,2.4)},
		{XMFLOAT3(0,1.4,2.4)},
		{XMFLOAT3(-0.784,1.4,2.4)},
		{XMFLOAT3(-1.4,0.784,2.4)},
		{XMFLOAT3(-1.4,0,2.4)},
		{XMFLOAT3(0,1.3375,2.53125)},
		{XMFLOAT3(-0.749,1.3375,2.53125)},
		{XMFLOAT3(-1.3375,0.749,2.53125)},
		{XMFLOAT3(-1.3375,0,2.53125)},
		{XMFLOAT3(0,1.4375,2.53125)},
		{XMFLOAT3(-0.805,1.4375,2.53125)},
		{XMFLOAT3(-1.4375,0.805,2.53125)},
		{XMFLOAT3(-1.4375,0,2.53125)},
		{XMFLOAT3(0,1.5,2.4)},
		{XMFLOAT3(-0.84,1.5,2.4)},
		{XMFLOAT3(-1.5,0.84,2.4)},
		{XMFLOAT3(-1.5,0,2.4)},

		// body: [192,576]
		{XMFLOAT3(0,-1.5,2.4)},
		{XMFLOAT3(0.84,-1.5,2.4)},
		{XMFLOAT3(1.5,-0.84,2.4)},
		{XMFLOAT3(1.5,0,2.4)},
		{XMFLOAT3(0,-1.75,1.875)},
		{XMFLOAT3(0.98,-1.75,1.875)},
		{XMFLOAT3(1.75,-0.98,1.875)},
		{XMFLOAT3(1.75,0,1.875)},
		{XMFLOAT3(0,-2,1.35)},
		{XMFLOAT3(1.12,-2,1.35)},
		{XMFLOAT3(2,-1.12,1.35)},
		{XMFLOAT3(2,0,1.35)},
		{XMFLOAT3(0,-2,0.9)},
		{XMFLOAT3(1.12,-2,0.9)},
		{XMFLOAT3(2,-1.12,0.9)},
		{XMFLOAT3(2,0,0.9)},
		{XMFLOAT3(-1.5,0,2.4)},
		{XMFLOAT3(-1.5,-0.84,2.4)},
		{XMFLOAT3(-0.84,-1.5,2.4)},
		{XMFLOAT3(0,-1.5,2.4)},
		{XMFLOAT3(-1.75,0,1.875)},
		{XMFLOAT3(-1.75,-0.98,1.875)},
		{XMFLOAT3(-0.98,-1.75,1.875)},
		{XMFLOAT3(0,-1.75,1.875)},
		{XMFLOAT3(-2,0,1.35)},
		{XMFLOAT3(-2,-1.12,1.35)},
		{XMFLOAT3(-1.12,-2,1.35)},
		{XMFLOAT3(0,-2,1.35)},
		{XMFLOAT3(-2,0,0.9)},
		{XMFLOAT3(-2,-1.12,0.9)},
		{XMFLOAT3(-1.12,-2,0.9)},
		{XMFLOAT3(0,-2,0.9)},
		{XMFLOAT3(1.5,0,2.4)},
		{XMFLOAT3(1.5,0.84,2.4)},
		{XMFLOAT3(0.84,1.5,2.4)},
		{XMFLOAT3(0,1.5,2.4)},
		{XMFLOAT3(1.75,0,1.875)},
		{XMFLOAT3(1.75,0.98,1.875)},
		{XMFLOAT3(0.98,1.75,1.875)},
		{XMFLOAT3(0,1.75,1.875)},
		{XMFLOAT3(2,0,1.35)},
		{XMFLOAT3(2,1.12,1.35)},
		{XMFLOAT3(1.12,2,1.35)},
		{XMFLOAT3(0,2,1.35)},
		{XMFLOAT3(2,0,0.9)},
		{XMFLOAT3(2,1.12,0.9)},
		{XMFLOAT3(1.12,2,0.9)},
		{XMFLOAT3(0,2,0.9)},
		{XMFLOAT3(0,1.5,2.4)},
		{XMFLOAT3(-0.84,1.5,2.4)},
		{XMFLOAT3(-1.5,0.84,2.4)},
		{XMFLOAT3(-1.5,0,2.4)},
		{XMFLOAT3(0,1.75,1.875)},
		{XMFLOAT3(-0.98,1.75,1.875)},
		{XMFLOAT3(-1.75,0.98,1.875)},
		{XMFLOAT3(-1.75,0,1.875)},
		{XMFLOAT3(0,2,1.35)},
		{XMFLOAT3(-1.12,2,1.35)},
		{XMFLOAT3(-2,1.12,1.35)},
		{XMFLOAT3(-2,0,1.35)},
		{XMFLOAT3(0,2,0.9)},
		{XMFLOAT3(-1.12,2,0.9)},
		{XMFLOAT3(-2,1.12,0.9)},
		{XMFLOAT3(-2,0,0.9)},
		{XMFLOAT3(0,-2,0.9)},
		{XMFLOAT3(1.12,-2,0.9)},
		{XMFLOAT3(2,-1.12,0.9)},
		{XMFLOAT3(2,0,0.9)},
		{XMFLOAT3(0,-2,0.45)},
		{XMFLOAT3(1.12,-2,0.45)},
		{XMFLOAT3(2,-1.12,0.45)},
		{XMFLOAT3(2,0,0.45)},
		{XMFLOAT3(0,-1.5,0.225)},
		{XMFLOAT3(0.84,-1.5,0.225)},
		{XMFLOAT3(1.5,-0.84,0.225)},
		{XMFLOAT3(1.5,0,0.225)},
		{XMFLOAT3(0,-1.5,0.15)},
		{XMFLOAT3(0.84,-1.5,0.15)},
		{XMFLOAT3(1.5,-0.84,0.15)},
		{XMFLOAT3(1.5,0,0.15)},
		{XMFLOAT3(-2,0,0.9)},
		{XMFLOAT3(-2,-1.12,0.9)},
		{XMFLOAT3(-1.12,-2,0.9)},
		{XMFLOAT3(0,-2,0.9)},
		{XMFLOAT3(-2,0,0.45)},
		{XMFLOAT3(-2,-1.12,0.45)},
		{XMFLOAT3(-1.12,-2,0.45)},
		{XMFLOAT3(0,-2,0.45)},
		{XMFLOAT3(-1.5,0,0.225)},
		{XMFLOAT3(-1.5,-0.84,0.225)},
		{XMFLOAT3(-0.84,-1.5,0.225)},
		{XMFLOAT3(0,-1.5,0.225)},
		{XMFLOAT3(-1.5,0,0.15)},
		{XMFLOAT3(-1.5,-0.84,0.15)},
		{XMFLOAT3(-0.84,-1.5,0.15)},
		{XMFLOAT3(0,-1.5,0.15)},
		{XMFLOAT3(2,0,0.9)},
		{XMFLOAT3(2,1.12,0.9)},
		{XMFLOAT3(1.12,2,0.9)},
		{XMFLOAT3(0,2,0.9)},
		{XMFLOAT3(2,0,0.45)},
		{XMFLOAT3(2,1.12,0.45)},
		{XMFLOAT3(1.12,2,0.45)},
		{XMFLOAT3(0,2,0.45)},
		{XMFLOAT3(1.5,0,0.225)},
		{XMFLOAT3(1.5,0.84,0.225)},
		{XMFLOAT3(0.84,1.5,0.225)},
		{XMFLOAT3(0,1.5,0.225)},
		{XMFLOAT3(1.5,0,0.15)},
		{XMFLOAT3(1.5,0.84,0.15)},
		{XMFLOAT3(0.84,1.5,0.15)},
		{XMFLOAT3(0,1.5,0.15)},
		{XMFLOAT3(0,2,0.9)},
		{XMFLOAT3(-1.12,2,0.9)},
		{XMFLOAT3(-2,1.12,0.9)},
		{XMFLOAT3(-2,0,0.9)},
		{XMFLOAT3(0,2,0.45)},
		{XMFLOAT3(-1.12,2,0.45)},
		{XMFLOAT3(-2,1.12,0.45)},
		{XMFLOAT3(-2,0,0.45)},
		{XMFLOAT3(0,1.5,0.225)},
		{XMFLOAT3(-0.84,1.5,0.225)},
		{XMFLOAT3(-1.5,0.84,0.225)},
		{XMFLOAT3(-1.5,0,0.225)},
		{XMFLOAT3(0,1.5,0.15)},
		{XMFLOAT3(-0.84,1.5,0.15)},
		{XMFLOAT3(-1.5,0.84,0.15)},
		{XMFLOAT3(-1.5,0,0.15)},

		// bottom: [960,1152]
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(1.425,0,0)},
		{XMFLOAT3(1.425,-0.798,0)},
		{XMFLOAT3(0.798,-1.425,0)},
		{XMFLOAT3(0,-1.425,0)},
		{XMFLOAT3(1.5,0,0.075)},
		{XMFLOAT3(1.5,-0.84,0.075)},
		{XMFLOAT3(0.84,-1.5,0.075)},
		{XMFLOAT3(0,-1.5,0.075)},
		{XMFLOAT3(1.5,0,0.15)},
		{XMFLOAT3(1.5,-0.84,0.15)},
		{XMFLOAT3(0.84,-1.5,0.15)},
		{XMFLOAT3(0,-1.5,0.15)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,-1.425,0)},
		{XMFLOAT3(-0.798,-1.425,0)},
		{XMFLOAT3(-1.425,-0.798,0)},
		{XMFLOAT3(-1.425,0,0)},
		{XMFLOAT3(0,-1.5,0.075)},
		{XMFLOAT3(-0.84,-1.5,0.075)},
		{XMFLOAT3(-1.5,-0.84,0.075)},
		{XMFLOAT3(-1.5,0,0.075)},
		{XMFLOAT3(0,-1.5,0.15)},
		{XMFLOAT3(-0.84,-1.5,0.15)},
		{XMFLOAT3(-1.5,-0.84,0.15)},
		{XMFLOAT3(-1.5,0,0.15)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,1.425,0)},
		{XMFLOAT3(0.798,1.425,0)},
		{XMFLOAT3(1.425,0.798,0)},
		{XMFLOAT3(1.425,0,0)},
		{XMFLOAT3(0,1.5,0.075)},
		{XMFLOAT3(0.84,1.5,0.075)},
		{XMFLOAT3(1.5,0.84,0.075)},
		{XMFLOAT3(1.5,0,0.075)},
		{XMFLOAT3(0,1.5,0.15)},
		{XMFLOAT3(0.84,1.5,0.15)},
		{XMFLOAT3(1.5,0.84,0.15)},
		{XMFLOAT3(1.5,0,0.15)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(0,0,0)},
		{XMFLOAT3(-1.425,0,0)},
		{XMFLOAT3(-1.425,0.798,0)},
		{XMFLOAT3(-0.798,1.425,0)},
		{XMFLOAT3(0,1.425,0)},
		{XMFLOAT3(-1.5,0,0.075)},
		{XMFLOAT3(-1.5,0.84,0.075)},
		{XMFLOAT3(-0.84,1.5,0.075)},
		{XMFLOAT3(0,1.5,0.075)},
		{XMFLOAT3(-1.5,0,0.15)},
		{XMFLOAT3(-1.5,0.84,0.15)},
		{XMFLOAT3(-0.84,1.5,0.15)},
		{XMFLOAT3(0,1.5,0.15)},

		// handle: [1152,1344]
		{XMFLOAT3(-1.5,0,2.25)},
		{XMFLOAT3(-1.5,-0.3,2.25)},
		{XMFLOAT3(-1.6,-0.3,2.025)},
		{XMFLOAT3(-1.6,0,2.025)},
		{XMFLOAT3(-2.5,0,2.25)},
		{XMFLOAT3(-2.5,-0.3,2.25)},
		{XMFLOAT3(-2.3,-0.3,2.025)},
		{XMFLOAT3(-2.3,0,2.025)},
		{XMFLOAT3(-3,0,2.25)},
		{XMFLOAT3(-3,-0.3,2.25)},
		{XMFLOAT3(-2.7,-0.3,2.025)},
		{XMFLOAT3(-2.7,0,2.025)},
		{XMFLOAT3(-3,0,1.8)},
		{XMFLOAT3(-3,-0.3,1.8)},
		{XMFLOAT3(-2.7,-0.3,1.8)},
		{XMFLOAT3(-2.7,0,1.8)},
		{XMFLOAT3(-1.6,0,2.025)},
		{XMFLOAT3(-1.6,0.3,2.025)},
		{XMFLOAT3(-1.5,0.3,2.25)},
		{XMFLOAT3(-1.5,0,2.25)},
		{XMFLOAT3(-2.3,0,2.025)},
		{XMFLOAT3(-2.3,0.3,2.025)},
		{XMFLOAT3(-2.5,0.3,2.25)},
		{XMFLOAT3(-2.5,0,2.25)},
		{XMFLOAT3(-2.7,0,2.025)},
		{XMFLOAT3(-2.7,0.3,2.025)},
		{XMFLOAT3(-3,0.3,2.25)},
		{XMFLOAT3(-3,0,2.25)},
		{XMFLOAT3(-2.7,0,1.8)},
		{XMFLOAT3(-2.7,0.3,1.8)},
		{XMFLOAT3(-3,0.3,1.8)},
		{XMFLOAT3(-3,0,1.8)},
		{XMFLOAT3(-3,0,1.8)},
		{XMFLOAT3(-3,-0.3,1.8)},
		{XMFLOAT3(-2.7,-0.3,1.8)},
		{XMFLOAT3(-2.7,0,1.8)},
		{XMFLOAT3(-3,0,1.35)},
		{XMFLOAT3(-3,-0.3,1.35)},
		{XMFLOAT3(-2.7,-0.3,1.575)},
		{XMFLOAT3(-2.7,0,1.575)},
		{XMFLOAT3(-2.65,0,0.9375)},
		{XMFLOAT3(-2.65,-0.3,0.9375)},
		{XMFLOAT3(-2.5,-0.3,1.125)},
		{XMFLOAT3(-2.5,0,1.125)},
		{XMFLOAT3(-1.9,0,0.6)},
		{XMFLOAT3(-1.9,-0.3,0.6)},
		{XMFLOAT3(-2,-0.3,0.9)},
		{XMFLOAT3(-2,0,0.9)},
		{XMFLOAT3(-2.7,0,1.8)},
		{XMFLOAT3(-2.7,0.3,1.8)},
		{XMFLOAT3(-3,0.3,1.8)},
		{XMFLOAT3(-3,0,1.8)},
		{XMFLOAT3(-2.7,0,1.575)},
		{XMFLOAT3(-2.7,0.3,1.575)},
		{XMFLOAT3(-3,0.3,1.35)},
		{XMFLOAT3(-3,0,1.35)},
		{XMFLOAT3(-2.5,0,1.125)},
		{XMFLOAT3(-2.5,0.3,1.125)},
		{XMFLOAT3(-2.65,0.3,0.9375)},
		{XMFLOAT3(-2.65,0,0.9375)},
		{XMFLOAT3(-2,0,0.9)},
		{XMFLOAT3(-2,0.3,0.9)},
		{XMFLOAT3(-1.9,0.3,0.6)},
		{XMFLOAT3(-1.9,0,0.6)},


		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);// static const unsigned short quadIndices[] =
		//{
		//0,2,1, // -x
		//	1,2,3,

		//	4,5,6, // +x
		//	5,7,6,

		//	0,1,5, // -y
		//	0,5,4,

		//	2,6,7, // +y
		//	2,7,3,

		//	0,4,6, // -z
		//	0,6,2,

		//	1,3,7, // +z
		//	1,7,5,
		//};
		static unsigned int quadIndices[] = {

			102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
			/* body */
		12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 ,
		 24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40 ,
		 /* lid */
	  96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,
	  0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117 ,
	  /* bottom */
   118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37 ,
   /* handle */
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56 ,
53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67 ,
/* spout */
68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83 ,
80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95
		};

		m_indexCount = ARRAYSIZE(cubeVertices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = quadIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC indexBufferDescription(sizeof(quadIndices), D3D11_BIND_INDEX_BUFFER);

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDescription, &indexBufferData, &m_indexBuffer));

		});
	CD3D11_RASTERIZER_DESC rasterStateDesc(D3D11_DEFAULT);

	rasterStateDesc.CullMode = D3D11_CULL_NONE;
	rasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;

	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateRasterizerState(
			&rasterStateDesc,
			m_rasterizerState.GetAddressOf()
		)
	);

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
		});
}

void AncientPottery::SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection)
{
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.view, DirectX::XMMatrixTranspose(view));

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.projection, DirectX::XMMatrixTranspose(projection));
	auto world = XMMatrixIdentity();
	//XMStoreFloat4x4(&m_MVPBufferData.model, world);

}

void AncientPottery::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_hullShader.Reset();
	m_domainShader.Reset();
	m_pixelShader.Reset();
	m_MVPBuffer.Reset();
	m_timeBuffer.Reset();
	m_cameraBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}
void AncientPottery::SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition)
{
	m_cameraBufferData.position = cameraPosition;
	m_cameraBufferData.padding = 1;
}

void AncientPottery::Update(DX::StepTimer const& timer)
{
	auto worldMatrix = DirectX::XMMatrixIdentity();
	////m_rotation.x +=timer.GetElapsedSeconds() * -0.2f;
	m_rotation.y += timer.GetElapsedSeconds() * 0.2f;
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z));
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z)));
	//worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationY(radians));

	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));

	//m_timeBufferData.time = timer.GetTotalSeconds();

	//DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, worldMatrix)));
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(worldMatrix));
}

void AncientPottery::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(
		m_MVPBuffer.Get(),
		0,
		NULL,
		&m_MVPBufferData,
		0,
		0,
		0
	);
	context->UpdateSubresource1(
		m_cameraBuffer.Get(),
		0,
		NULL,
		&m_cameraBufferData,
		0,
		0,
		0
	);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPosition);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);
	//context->RSSetState(m_rasterizerState.Get());
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);
	context->VSSetConstantBuffers1(
		0,
		1,
		m_MVPBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	////// Attach our vertex shader.
	context->DSSetShader(
		m_domainShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->DSSetConstantBuffers1(
		0,
		1,
		m_MVPBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->DSSetConstantBuffers1(
		1,
		1,
		m_cameraBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);
	context->HSSetShader(m_hullShader.Get(), nullptr, 0);

	context->GSSetShader(
		nullptr,
		nullptr,
		0
	);

	// Draw the objects.
		// Draw the objects.
	context->Draw(
		m_indexCount,
		0

	);

}
