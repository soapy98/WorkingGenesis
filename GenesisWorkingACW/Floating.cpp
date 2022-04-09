#include "pch.h"
#include "Floating.h"
#include <Content/DDSTextureLoader.h>
using namespace GenesisWorkingACW;
using namespace DirectX;

Floating::Floating(const std::shared_ptr<DX::DeviceResources>& deviceResources)
	: m_deviceResources(deviceResources), m_position(0.f, 6.5f, 1.f), m_rotation(40.0f, 50.0f, 0.f), m_scale(1.5f, 1.5f, 1.5f), m_loadingComplete(false), m_indexCount(0)
{
	CreateDeviceDependentResources();
}

void Floating::CreateDeviceDependentResources()
{
	auto loadVSTask = DX::ReadDataAsync(L"FloatVS.cso");
	auto loadHSTask = DX::ReadDataAsync(L"FloatHS.cso");
	auto loadDSTask = DX::ReadDataAsync(L"FloatDS.cso");
	auto loadPSTask = DX::ReadDataAsync(L"FloatPS.cso");

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
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

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

	auto createHSTask = loadHSTask.then([this](const std::vector<byte>& fileData)
		{
			DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateHullShader(&fileData[0], fileData.size(), nullptr, &m_hullShader));
		});

	auto createDSTask = loadDSTask.then([this](const std::vector<byte>& fileData)
		{
			DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateDomainShader(&fileData[0], fileData.size(), nullptr, &m_domainShader));
		});

	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);

		D3D11_SAMPLER_DESC samplerWrapDescription;

		samplerWrapDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerWrapDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerWrapDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerWrapDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerWrapDescription.MipLODBias = 0.0f;
		samplerWrapDescription.MaxAnisotropy = 1;
		samplerWrapDescription.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerWrapDescription.BorderColor[0] = 0.0f;
		samplerWrapDescription.BorderColor[1] = 0.0f;
		samplerWrapDescription.BorderColor[2] = 0.0f;
		samplerWrapDescription.BorderColor[3] = 0.0f;
		samplerWrapDescription.MinLOD = 0.0f;
		samplerWrapDescription.MaxLOD = D3D11_FLOAT32_MAX;


		CD3D11_BUFFER_DESC MVPBufferDescription(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&MVPBufferDescription, nullptr, &m_MVPBuffer));

		CD3D11_BUFFER_DESC cameraBufferDescription(sizeof(CameraConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&cameraBufferDescription, nullptr, &m_CamBuffer));

		});

	auto createPlaneTask = (createPSTask && createDSTask && createHSTask && createVSTask).then([this]() {
		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPosition cubeVertices[] =
		{

			{XMFLOAT3(-0.5f, -0.5f, -0.5f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f)},
			{XMFLOAT3(0.5f, -0.5f, -0.5f)},
			{XMFLOAT3(0.5f, -0.5f,  0.5f)},
			{XMFLOAT3(0.5f,  0.5f, -0.5f)},
			{XMFLOAT3(0.5f,  0.5f,  0.5f)},

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

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short quadIndices[] =
		{
		0,2,1, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};
		m_indexCount = ARRAYSIZE(cubeVertices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = quadIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(quadIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);
		CD3D11_RASTERIZER_DESC rasterStateDesc(D3D11_DEFAULT);

		rasterStateDesc.CullMode = D3D11_CULL_NONE;
		rasterStateDesc.FillMode = D3D11_FILL_SOLID;

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateRasterizerState(
				&rasterStateDesc,
				m_rast.GetAddressOf()
			)
		);
		CD3D11_BUFFER_DESC timeconstantBufferDesc(sizeof(TotalTimeConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&timeconstantBufferDesc,
				nullptr,
				&m_timeBuffer
			)
		);

		DX::ThrowIfFailed(CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets\\BaseCol.dds", nullptr, &m_potteryTexture));
		shader = m_potteryTexture.Get();
		D3D11_SAMPLER_DESC sampDesc = {};
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.MaxAnisotropy = 0;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&sampDesc, &m_sampler));
		});

	createPlaneTask.then([this]() {
		m_loadingComplete = true;
		});

}

void Floating::SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection)
{
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.view, DirectX::XMMatrixTranspose(view));

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.projection, DirectX::XMMatrixTranspose(projection));
}

void Floating::SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition)
{
	m_CamBufferData.position = cameraPosition;
	m_CamBufferData.padding = 1;
}


void Floating::Update(DX::StepTimer const& timer)
{
	auto worldMatrix = DirectX::XMMatrixIdentity();
	m_timeBufferData.time = timer.GetTotalSeconds();
	m_timeBufferData.padding = XMFLOAT3(0, 0, 0);
	m_rotation.z += timer.GetElapsedSeconds()*0.2;
	m_rotation.y += timer.GetElapsedSeconds() * 0.2;
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z));
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z)));
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(worldMatrix));
}

void Floating::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare constant buffers to send it to the graphics device.
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
		m_CamBuffer.Get(),
		0,
		NULL,
		&m_CamBufferData,
		0,
		0,
		0
	);
	context->UpdateSubresource1(
		m_timeBuffer.Get(),
		0,
		NULL,
		&m_timeBufferData,
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

	//context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());
	context->RSSetState(m_rast.Get());
	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_MVPBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);



	context->HSSetShader(
		m_hullShader.Get(),
		nullptr,
		0
	);

	context->DSSetShader(
		m_domainShader.Get(),
		nullptr,
		0
	);

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
		m_CamBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->DSSetConstantBuffers1(
		2,
		1,
		m_timeBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	context->GSSetShader(
		nullptr,
		nullptr,
		0
	);
	auto s = m_potteryTexture.Get();
	context->PSSetShaderResources(0, 1, &shader);
	auto sampler = m_sampler.Get();
	context->PSSetSamplers(0, 1, &sampler);
	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);


	// Draw the objects.
	context->Draw(
		m_indexCount,
		0
	);
}



void Floating::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_hullShader.Reset();
	m_domainShader.Reset();
	m_pixelShader.Reset();
	m_MVPBuffer.Reset();
	m_CamBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}