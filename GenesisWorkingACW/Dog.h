#pragma once
#pragma once
#include <Content/ShaderStructures.h>
#include <Common/DeviceResources.h>
#include <Common/StepTimer.h>
namespace Genesis
{
	class Dog
	{
	public:
		Dog(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection);
		void ReleaseDeviceDependentResources();
		void SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition);
		void Update(DX::StepTimer const& timer);
		void Render();

	private:

		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_rotation;
		DirectX::XMFLOAT3 m_scale;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;

		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11HullShader>	m_hullShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_domainShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;

		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_MVPBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_timeBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_cameraBuffer;

		ModelViewProjectionConstantBuffer			m_MVPBufferData;
		//TotalTimeConstantBuffer						m_timeBufferData;
		CameraConstantBuffer				m_cameraBufferData;
		uint32	m_indexCount;

		bool	m_loadingComplete;

	};
}

