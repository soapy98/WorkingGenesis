#pragma once

namespace GenesisWorkingACW
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};
	struct VertexPosition
	{
		DirectX::XMFLOAT3 position;
	};
	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};
	struct CameraConstantBuffer
	{
		DirectX::XMFLOAT3 position;
		float padding;
	};

	struct TotalTimeConstantBuffer
	{
		float time;
		DirectX::XMFLOAT3 padding;
	};

	struct DeltaTimeConstantBuffer
	{
		float dt;
		DirectX::XMFLOAT3 padding;
	};
	struct InverseConstantBuffer
	{
		DirectX::XMFLOAT4X4 inverse;
	};

}