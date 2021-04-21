#include "pch.h"
#include "StarrySky.h"
#include <Common/DirectXHelper.h>
#include <Common/StepTimer.h>

using namespace GenesisWorkingACW;
using namespace DirectX;
StarrySky::StarrySky(const std::shared_ptr<DX::DeviceResources>& deviceResources) :m_deviceResources(deviceResources), m_position(0, 0, 0), m_rotation(0.0f, 0.0f, 0.0f), m_scale(1.4f, 3.4f, 3.4f)
{
	CreateDeviceDependentResources();
}

void StarrySky::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"StarryVS.cso");
	auto loadPSTask = DX::ReadDataAsync(L"StarryPS.cso");

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

		});


	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPosition cubeVertices[] = {
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
		);


		m_indexCount = ARRAYSIZE(cubeVertices);


		});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
		});
}

void StarrySky::SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection)
{
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.view, DirectX::XMMatrixTranspose(view));

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.projection, DirectX::XMMatrixTranspose(projection));

}
void StarrySky::SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition)
{
	m_cameraBufferData.position = cameraPosition;
	m_cameraBufferData.padding = 1;
}
void StarrySky::Render()
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



	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	context->VSSetConstantBuffers(
		0,
		1,
		m_MVPBuffer.GetAddressOf()
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	context->DSSetShader(
		nullptr,
		nullptr,
		0
	);
	context->HSSetShader(
		nullptr,
		nullptr,
		0
	);
	context->GSSetShader(
		nullptr,
		nullptr,
		0
	);

	// Draw the objects.
	context->Draw(
		m_indexCount,
		0

	);
}
void StarrySky::Update(DX::StepTimer const& timer)
{
	auto worldMatrix = DirectX::XMMatrixIdentity();
	//m_rotation.x += -0.2f * timer.GetElapsedSeconds();
	m_rotation.y += 2.f * timer.GetElapsedSeconds();
	m_position.z += sin(timer.GetElapsedSeconds());
	//m_timeBufferData.time = timer.GetTotalSeconds();
	//m_timeBufferData.padding = XMFLOAT3(0, 0, 0);
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z));
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z)));

	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));


	DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(worldMatrix));
}

void StarrySky::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_hullShader.Reset();
	m_domainShader.Reset();
	m_pixelShader.Reset();
	m_MVPBuffer.Reset();
	//m_CamBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}