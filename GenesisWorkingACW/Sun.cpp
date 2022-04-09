#include "pch.h"
#include "Sun.h"

#include <Common/DirectXHelper.h>

using namespace GenesisWorkingACW;
using namespace DirectX;
Sun::Sun(const std::shared_ptr<DX::DeviceResources>& deviceResources) :m_deviceResources(deviceResources), m_position(0.f, 0.f, 10.5f), m_rotation(0.0f, 0.0f, 0.0f), m_scale(20.f, 20.f, 20.f)
{
	CreateDeviceDependentResources();
}


void Sun::Render()
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
	m_deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(1, &g_pRenderableTexture_RTV, g_pDepthStencilView);
	context->OMSetDepthStencilState(g_stencil_stateSky, 1);
	context->RSSetState(g_pRasterSateSky);
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
	auto rtv = m_deviceResources->GetBackBufferRenderTargetView();
	context->OMSetRenderTargets(1, &rtv, g_pDepthStencilView);
	context->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
	//context->RSSetState(g_pRasterSateSky);


	context->VSSetShader(
		m_rendVS.Get(),
		nullptr,
		0
	);
	context->PSSetShaderResources(0, 1, &g_pRenderableTexure_SRV);
	auto sampler = m_sampler.Get();
	context->PSSetSamplers(0, 1, &sampler);
	context->OMSetDepthStencilState(g_stencil_stateCube, 1);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(
		0,
		1,
		m_MVPBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	context->VSSetConstantBuffers1(
		1,
		1,
		m_camBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);


	// Attach our pixel shader.
	context->PSSetShader(
		m_renderPS.Get(),
		nullptr,
		0
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
	const FLOAT clear[4] = { 0.0,0.0,0.0,0.0 };
	context->ClearRenderTargetView(g_pRenderableTexture_RTV, clear); // Clear the rendered texture



}
void Sun::SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition)
{
	m_camBufferData.position = cameraPosition;
	m_camBufferData.padding = 1;
}

void Sun::CreateDeviceDependentResources()
{

	auto loadVSTask = DX::ReadDataAsync(L"ImplicitVS.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SunPS.cso");

	auto loadVS2Task = DX::ReadDataAsync(L"RenderVS.cso");
	auto loadPS2Task = DX::ReadDataAsync(L"RenderTexturePS.cso");
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
	auto createVSTask2 = loadVS2Task.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_rendVS
			)
		);
		});
	auto createPSTask2 = loadPS2Task.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_renderPS
			)
		);
		});
	//// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask && createPSTask2 && createVSTask2).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPosition cubeVertices[] =
		{

		{DirectX::XMFLOAT3(-50.5f, -50.5f, 0.0f)},
			{DirectX::XMFLOAT3(-50.5f, 50.5f, 0.0f)},
			{DirectX::XMFLOAT3(50.5f,  -50.5f,0.0f)},
			{DirectX::XMFLOAT3(50.5f,  50.5f, 0.0f)},
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
	auto outputSize = m_deviceResources->GetOutputSize();

	D3D11_RASTERIZER_DESC rasterizer;
	rasterizer.CullMode = D3D11_CULL_NONE;
	rasterizer.FillMode = D3D11_FILL_SOLID;

	rasterizer.ScissorEnable = false;
	rasterizer.DepthBias = 0;
	rasterizer.DepthBiasClamp = 0.0f;
	rasterizer.DepthBiasClamp = 0.0f;
	rasterizer.DepthClipEnable = false;
	rasterizer.MultisampleEnable = false;
	rasterizer.SlopeScaledDepthBias = 0.0f;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizer, &g_pRasterSateSky);
	rasterizer.CullMode = D3D11_CULL_FRONT;

	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizer, &g_pRasterSateCube);
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = outputSize.Width;
	descDepth.Height = outputSize.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateTexture2D(&descDepth, nullptr, &g_pDepthStencil));


	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView));

	D3D11_TEXTURE2D_DESC ren_desc;
	ZeroMemory(&ren_desc, sizeof(ren_desc));
	ren_desc.Width = outputSize.Width;
	ren_desc.Height = outputSize.Height;
	ren_desc.MipLevels = 1;
	ren_desc.ArraySize = 1;
	ren_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	ren_desc.SampleDesc.Count = 1;
	ren_desc.Usage = D3D11_USAGE_DEFAULT;
	ren_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	ren_desc.CPUAccessFlags = 0;
	ren_desc.MiscFlags = 0;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateTexture2D(&ren_desc, nullptr, &g_pRenderableTexture));

	D3D11_RENDER_TARGET_VIEW_DESC renderable_RTV_Desc;
	renderable_RTV_Desc.Format = ren_desc.Format;
	renderable_RTV_Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderable_RTV_Desc.Texture2D.MipSlice = 0;

	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateRenderTargetView(g_pRenderableTexture, nullptr, &g_pRenderableTexture_RTV));

	D3D11_SHADER_RESOURCE_VIEW_DESC rend_SRV_Desc;
	rend_SRV_Desc.Format = renderable_RTV_Desc.Format;
	rend_SRV_Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	rend_SRV_Desc.Texture2D.MostDetailedMip = 0;
	rend_SRV_Desc.Texture2D.MipLevels = 1;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateShaderResourceView(g_pRenderableTexture, &rend_SRV_Desc, &g_pRenderableTexure_SRV));
	D3D11_SAMPLER_DESC sampDesc = {};
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MaxAnisotropy = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateSamplerState(&sampDesc, &m_sampler));

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&dsDesc, &g_stencil_stateSky);

	dsDesc.DepthEnable = true;
	m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&dsDesc, &g_stencil_stateCube);
	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
		});
}

void Sun::SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection)
{
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.view, DirectX::XMMatrixTranspose(view));

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.projection, DirectX::XMMatrixTranspose(projection));
	DirectX::XMStoreFloat4x4(&m_inverseBufferData.inverse, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, view)));
	//XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(world));
}

void Sun::ReleaseDeviceDependentResources()
{
	m_inputLayout->Release();
	m_vertexBuffer->Release();
	m_indexBuffer->Release();

	m_vertexShader->Release();

	m_pixelShader->Release();

	m_MVPBuffer->Release();
}

void Sun::Update(DX::StepTimer const& timer)
{
	auto worldMatrix = DirectX::XMMatrixIdentity();

	/*m_rotation.x += timer.GetTotalSeconds();
	m_rotation.y += timer.GetTotalSeconds();*/

	//m_position.y += sin(timer.GetTotalSeconds());
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z));
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z)));

	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));
	m_timeBufferData.time = timer.GetTotalSeconds();
	m_timeBufferData.padding = XMFLOAT3(0, 0, 0);
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(worldMatrix));
}
