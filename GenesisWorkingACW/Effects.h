#pragma once
#include <Common/DeviceResources.h>
#include <Common/StepTimer.h>
#include <Content/ShaderStructures.h>
#include <string>
namespace GenesisWorkingACW {
	class Effects
	{
	public:
		Effects(const std::shared_ptr<DX::DeviceResources>& deviceResources, std::wstring name);
		void CreateDeviceDependentResources();
		void SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection);
		void ReleaseDeviceDependentResources();
		void SetPos(float x, float y, float z) { m_position = DirectX::XMFLOAT3(x, y, z); }
		void SetRot(float x, float y, float z) { m_rotation = DirectX::XMFLOAT3(x, y, z); }
		void SetScale(float x, float y, float z) { m_scale = DirectX::XMFLOAT3(x, y, z); }
		void Update(DX::StepTimer const& timer);
		void Render();
		void SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition);
		void RenderTexture();
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_rotation;
		DirectX::XMFLOAT3 m_scale;
		ModelViewProjectionConstantBuffer	m_constantBufferData;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_rendVS;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>	m_geoShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_domainShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_renderPS;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_MVPBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_camBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_inverseBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_timeBuffer;

		ModelViewProjectionConstantBuffer			m_MVPBufferData;
		CameraConstantBuffer m_camBufferData;
		TotalTimeConstantBuffer m_timeBufferData;
		InverseConstantBuffer m_inverseBufferData;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_potteryTexture;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>m_sampler;
		ID3D11DepthStencilState* g_stencil_stateCube = nullptr;
		ID3D11DepthStencilState* g_stencil_stateSky = nullptr;
		ID3D11ShaderResourceView* shader;
		ID3D11RasterizerState* g_pRasterSateSky = nullptr;
		ID3D11RasterizerState* g_pRasterSateCube = nullptr;
		ID3D11Texture2D* g_pRenderableTexture = nullptr;
		ID3D11RenderTargetView* g_pRenderableTexture_RTV = nullptr;
		ID3D11ShaderResourceView* g_pRenderableTexure_SRV = nullptr;
		ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
		ID3D11Texture2D* g_pDepthStencil = nullptr;
		uint32										m_indexCount;
		bool										m_loadingComplete;
		std::wstring _name;
	};
}
