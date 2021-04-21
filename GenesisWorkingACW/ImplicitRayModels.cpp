#include "pch.h"
#include "ImplicitRayModels.h"
#include <Common/DirectXHelper.h>
using namespace GenesisWorkingACW;
using namespace DirectX;
ImplicitRay::ImplicitRay(const std::shared_ptr<DX::DeviceResources>& deviceResources) :m_deviceResources(deviceResources), m_position(0.f, 0.f, 5.5f), m_rotation(0.0f, 0.0f, 0.0f), m_scale(1.f, 1.f, 1.f)
{
	CreateDeviceDependentResources();
}


void ImplicitRay::Render()
{

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
		m_camBuffer.Get(),
		0,
		NULL,
		&m_camBufferData,
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
	context->UpdateSubresource1(
		m_inverseBuffer.Get(),
		0,
		NULL,
		&m_inverseBufferData,
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


	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

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

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);
	context->PSSetConstantBuffers1(
		0,
		1,
		m_MVPBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->PSSetConstantBuffers1(
		1,
		1,
		m_inverseBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->PSSetConstantBuffers1(
		2,
		1,
		m_camBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->PSSetConstantBuffers1(
		3,
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

	// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0, 0
	);

}
void ImplicitRay::SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition)
{
	m_camBufferData.position = cameraPosition;
	m_camBufferData.padding = 1;
}
void ImplicitRay::CreateDeviceDependentResources()
{
	auto loadVSTask = DX::ReadDataAsync(L"ImplicitVS.cso");
	auto loadPSTask = DX::ReadDataAsync(L"RayMarchObjPS.cso");
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
				&m_camBuffer
			)
		);
		CD3D11_BUFFER_DESC inv(sizeof(InverseConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&inv,
				nullptr,
				&m_inverseBuffer
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
		});
	//

	//// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPosition cubeVertices[] =
		{
			{DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f)},
			{DirectX::XMFLOAT3(-0.5f, 0.5f,  0.0f)},
			{DirectX::XMFLOAT3(0.5f,  -0.5f, 0.0f)},
			{DirectX::XMFLOAT3(0.5f,  0.5f,  0.0f)},
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
			0, 1, 2,
			3, 2, 1
		};
		m_indexCount = ARRAYSIZE(quadIndices);

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
		});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
		});
}

void ImplicitRay::SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection)
{
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.view, DirectX::XMMatrixTranspose(view));

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.projection, DirectX::XMMatrixTranspose(projection));
	DirectX::XMStoreFloat4x4(&m_inverseBufferData.inverse, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, view)));
	//XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(world));
}

void ImplicitRay::ReleaseDeviceDependentResources()
{
	m_inputLayout->Release();
	m_vertexBuffer->Release();
	m_indexBuffer->Release();

	m_vertexShader->Release();

	m_pixelShader->Release();

	m_MVPBuffer->Release();
}

void ImplicitRay::Update(DX::StepTimer const& timer)
{
	auto worldMatrix = DirectX::XMMatrixIdentity();

	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z));
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z)));

	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));
	m_timeBufferData.time = timer.GetTotalSeconds();
	m_timeBufferData.padding = XMFLOAT3(0, 0, 0);
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(worldMatrix));
}
