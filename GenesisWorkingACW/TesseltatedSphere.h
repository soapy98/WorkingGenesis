#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\Content\ShaderStructures.h"
#include "..\Common\StepTimer.h"

#include <DirectXMath.h>
namespace GenesisWorkingACW {
	class TesseltatedSphere
	{
	public:
		TesseltatedSphere(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void SetPos(float x, float y, float z) { m_position = DirectX::XMFLOAT3(x, y, z); }
		void CreateDeviceDependentResources();
		void SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection);
		void SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition);
		void ReleaseDeviceDependentResources();
		void SetTessShader(bool tess) { camTess = tess; }
		void Update(DX::StepTimer const& timer);
		void Render();

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
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_camtessShader;
		Microsoft::WRL::ComPtr<ID3D11HullShader>	m_hullShader;
		Microsoft::WRL::ComPtr<ID3D11HullShader>	m_tesshullShader;
		Microsoft::WRL::ComPtr<ID3D11DomainShader>	m_domainShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_MVPBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_CamBuffer;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rast;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samp;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_timeBuffer;
		TotalTimeConstantBuffer m_timeBufferData;
		ModelViewProjectionConstantBuffer			m_MVPBufferData;
		CameraConstantBuffer m_CamBufferData;
		uint32										m_indexCount;
		bool										m_loadingComplete;
		bool camTess;
	};
}
