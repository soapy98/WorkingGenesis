#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "StarrySky.h"
#include "Camera.h"
#include "BillBoard.h"
#include <AncientPottery.h>
#include <ImplicitRayModels.h>
#include "ShinyObjects.h"
#include "TesseltatedSphere.h"
#include "Torus.h"
#include "Terrain.h"
#include "Floating.h"
#include <Explode.h>
#include <Effects.h>
#include <BlackHole.h>
#include <Sun.h>


namespace GenesisWorkingACW
{
	using namespace Windows::System;
	using namespace Windows::UI::Core;
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		bool IsTracking() { return m_tracking; }
		void CheckInputCameraMovement(DX::StepTimer const& timer);
		bool QueryKeyPressed(VirtualKey key);

	private:

	private:
		std::unique_ptr<Camera> m_cam;
		std::unique_ptr<StarrySky> m_StarrySky;
		std::unique_ptr<BillBoard>m_BillBoard;
		std::unique_ptr<Torus>m_torus;
		std::unique_ptr<ImplicitRay> m_ImplicitRay;
		std::unique_ptr<ShinyObjs> m_ShinyObjs;
		std::vector<std::unique_ptr<Explode>>m_explode;
		std::unique_ptr<AncientPottery> m_pottery;
		std::unique_ptr<TesseltatedSphere> m_CamTessSphere;
		std::unique_ptr<TesseltatedSphere> m_TessSphere;
		std::unique_ptr<TesseltatedSphere> m_face;
		std::vector<std::unique_ptr<Floating>> m_float;
		std::unique_ptr<Terrain>m_terrain;

		std::unique_ptr<Sun>m_sun;
		std::unique_ptr<BlackHole> m_blackHole;
		std::unique_ptr<Effects> m_frac;
		std::unique_ptr<Effects> m_stars;



		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rast;
		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
	};
}

