#include "pch.h"
#include "StarrySky.h"
#include <Common/DirectXHelper.h>
#include <Common/StepTimer.h>

using namespace GenesisWorkingACW;
using namespace DirectX;
StarrySky::StarrySky(const std::shared_ptr<DX::DeviceResources>& deviceResources) :m_deviceResources(deviceResources), m_position(0, 0, 0), m_rotation(0.0f, 0.0f, 0.0f), m_scale(.4f, .4f, .4f)
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
		CD3D11_BUFFER_DESC timeCB(sizeof(TotalTimeConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&timeCB,
				nullptr,
				&m_timeBuffer
			)
		);

		});


	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		float PI = 3.14159;
		// Load mesh vertices. Each vertex has a position and a color.
		static  VertexPosition cubeVertices[600] = {};
		XMFLOAT3 cent = { 0,0,-5 };
		int count = 0;
		for (int i = 0; count < 600; i++)
		{
			auto randX = -5 + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 5.f));
			auto randY = 1.5f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / (5)));
			auto randZ = static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX / 5));
			cubeVertices[count].position = XMFLOAT3((cent.x + 6 * cos(count) + 5) + randX, randY, (cent.z + 6 * sin(count) + 5) + randZ);
			count++;
		}



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
	context->UpdateSubresource1(
		m_timeBuffer.Get(),
		0,
		NULL,
		&m_timeData,
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

	context->VSSetConstantBuffers(
		1,
		1,
		m_timeBuffer.GetAddressOf()
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
		m_timeBuffer.GetAddressOf()
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
	////m_rotation.x += -0.2f * timer.GetElapsedSeconds();
	//m_rotation.y += 2.f * timer.GetElapsedSeconds();
	//m_position.z += sin(timer.GetElapsedSeconds());
	m_timeData.time = timer.GetTotalSeconds();
	m_timeData.padding = XMFLOAT3(0, 0, 0);
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