#include "pch.h"
#include "Explode.h"
#include <Common/DirectXHelper.h>

using namespace GenesisWorkingACW;
using namespace DirectX;
Explode::Explode(const std::shared_ptr<DX::DeviceResources>& deviceResources) :m_deviceResources(deviceResources), m_position(0, 0, 4), m_rotation(0.0f, 0.0f, 0.0f), m_scale(1.4f, 0.4f, 0.4f)
{
	CreateDeviceDependentResources();
}

void Explode::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"ExplodeVS.cso");
	auto loadHSTask = DX::ReadDataAsync(L"ExplodeHS.cso");
	auto loadDSTask = DX::ReadDataAsync(L"ExplodeDS.cso");
	auto loadPSTask = DX::ReadDataAsync(L"ExplodePS.cso");
	auto loadGSTask = DX::ReadDataAsync(L"ExplodeGS.cso");

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
		CD3D11_BUFFER_DESC timeconstantBufferDesc(sizeof(TotalTimeConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&timeconstantBufferDesc,
				nullptr,
				&m_timebuffer
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
	auto createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateGeometryShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_geoShader
			)
		);
		});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask && createGSTask && createDSTask && createHSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static  VertexPosition cubeVertices[] = { {XMFLOAT3(-0.5f, -0.5f, -0.5f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f)},
			{XMFLOAT3(0.5f, -0.5f, -0.5f)},
			{XMFLOAT3(0.5f, -0.5f,  0.5f)},
			{XMFLOAT3(0.5f,  0.5f, -0.5f)},
			{XMFLOAT3(0.5f,  0.5f,  0.5f)} };


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
		static const unsigned short cubeIndices[] =
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

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);


		});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
		});
}

void Explode::SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection)
{
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.view, DirectX::XMMatrixTranspose(view));

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.projection, DirectX::XMMatrixTranspose(projection));
	auto world = XMMatrixIdentity();

}
void Explode::SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition)
{
	m_cameraBufferData.position = cameraPosition;
	m_cameraBufferData.padding = 1;
}


void Explode::Update(DX::StepTimer const& timer)
{
	auto worldMatrix = DirectX::XMMatrixIdentity();

	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z));
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z)));

	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));
	m_timeBufferData.time = timer.GetTotalSeconds();
	m_timeBufferData.padding = XMFLOAT3(0, 0, 0);

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(worldMatrix));
}

void Explode::Render()
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
	context->UpdateSubresource1(
		m_timebuffer.Get(),
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

	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);


	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
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
	context->VSSetConstantBuffers(
		1,
		1,
		m_cameraBuffer.GetAddressOf()
	);
	context->VSSetConstantBuffers(
		2,
		1,
		m_timebuffer.GetAddressOf()
	);
	context->GSSetShader(
		m_geoShader.Get(),
		nullptr,
		0
	);
	context->GSSetConstantBuffers(
		0,
		1,
		m_MVPBuffer.GetAddressOf()
	);
	context->GSSetConstantBuffers(
		1,
		1,
		m_cameraBuffer.GetAddressOf()
	);
	context->GSSetConstantBuffers(
		2,
		1,
		m_timebuffer.GetAddressOf()
	);
	context->DSSetShader(
		m_domainShader.Get(),
		nullptr,
		0
	);
	context->DSSetConstantBuffers(
		0,
		1,
		m_MVPBuffer.GetAddressOf()
	);
	context->DSSetConstantBuffers(
		1,
		1,
		m_cameraBuffer.GetAddressOf()
	);
	context->DSSetConstantBuffers(
		2,
		1,
		m_timebuffer.GetAddressOf()
	);
	context->HSSetShader(
		m_hullShader.Get(),
		nullptr,
		0
	);
	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);
	context->PSSetConstantBuffers(
		0,
		1,
		m_MVPBuffer.GetAddressOf()
	);
	context->PSSetConstantBuffers(
		1,
		1,
		m_cameraBuffer.GetAddressOf()
	);

	context->PSSetConstantBuffers(
		2,
		1,
		m_timebuffer.GetAddressOf()
	);
	
	// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0, 0

	);
	//context->Draw(
	//	m_indexCount,
	//	0

	//);
}

void Explode::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	//m_hullShader.Reset();
	m_domainShader.Reset();
	m_pixelShader.Reset();
	m_MVPBuffer.Reset();
	//m_CamBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}