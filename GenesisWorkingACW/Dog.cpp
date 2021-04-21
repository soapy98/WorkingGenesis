#include "pch.h"
#include "Dog.h"
#include <Common/DirectXHelper.h>
using namespace Genesis;
using namespace DirectX;

Dog::Dog(const std::shared_ptr<DX::DeviceResources>& deviceResources) :m_deviceResources(deviceResources), m_position(0.f, 0.f, 5.0f), m_rotation(10.f, 10.0f, 10.0f), m_scale(0.2f, .2f, .2f), m_loadingComplete(false), m_indexCount(0)
{
	CreateDeviceDependentResources();
}

void Dog::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"PotteryVS.cso");
	auto loadHSTask = DX::ReadDataAsync(L"PotteryHS.cso");
	auto loadDSTask = DX::ReadDataAsync(L"PotteryDS.cso");
	auto loadPSTask = DX::ReadDataAsync(L"PotteryPS.cso");

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
		});
	auto createHSTask = loadHSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateHullShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_hullShader
			)
		);
		});
	auto createDSTask = loadDSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateDomainShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_domainShader
			)
		);
		});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask && createHSTask && createDSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPosition cubeVertices[] = {
		{ XMFLOAT3(-1.0703 ,60.0632, 42.5839) },
		{ XMFLOAT3(0.0000, 60.5208 ,43.2929) },
		{ XMFLOAT3(0.0000 ,62.0871, 38.4269) },
		{ XMFLOAT3(-1.2073, 61.7317, 38.5023) },
		{ XMFLOAT3(-1.4735 ,62.9793 ,35.3937) },
		{ XMFLOAT3(0.0000, 63.0023, 35.9347) },
		{ XMFLOAT3(0.0000, 66.5374, 33.6997) },
		{ XMFLOAT3(-1.6151, 66.4783 ,33.5434) },
		{ XMFLOAT3(0.0000, 68.1502, 30.5125) },
		{ XMFLOAT3(-2.0337 ,68.0208, 30.3775) },
		{ XMFLOAT3(0.0000, 68.6399 ,24.5492) },
		{ XMFLOAT3(-1.4123 ,68.4196, 25.0142) },
		{ XMFLOAT3(0.0000, 67.5486, 21.6502) },
		{ XMFLOAT3(-3.3688 ,66.7840 ,22.0988) },
		{ XMFLOAT3(0.0000, 63.0286 ,16.2640) },
		{ XMFLOAT3(-4.9529 ,61.9067 ,17.5324) },
		{ XMFLOAT3(-5.2209, 58.7555,13.7239) },
		{ XMFLOAT3(0.0000, 59.8812, 12.3276) },
		{ XMFLOAT3(0.0000 ,55.6331 ,7.2884) },
		{ XMFLOAT3(-5.4912, 53.5690, 7.0532) },
		{ XMFLOAT3(0.0000, 52.7779 ,-4.5080) },
		{ XMFLOAT3(-5.1013, 50.5662, -4.6607) },
		{ XMFLOAT3(0.0000, 51.3881 ,-18.0608) },
		{ XMFLOAT3(-4.2878 ,49.8122, -16.8214) },
		{ XMFLOAT3(0.0000, 51.1005, -25.0311) },
		{ XMFLOAT3(-3.7827, 49.8836, -26.7780) },
		{ XMFLOAT3(-3.5104, 46.1778, -31.3639) },
		{ XMFLOAT3(0.0000, 45.9378 ,-32.2941) },
		{ XMFLOAT3(0.0000, 40.1842, -34.0518) },
		{ XMFLOAT3(-3.1590, 39.3728, -33.9397) },
		{ XMFLOAT3(0.0000, 31.3032, -23.0912) },
		{ XMFLOAT3(-1.5250, 31.9297, -23.0912) },
		{ XMFLOAT3(0.0000, 28.2578, -12.5282) },
		{ XMFLOAT3(-3.9890, 28.8871 ,-12.8093) },
		{ XMFLOAT3(0.0000, 25.7754, -3.9972) },
		{ XMFLOAT3(-6.1336 ,26.9997, -4.1936) },
		{ XMFLOAT3(0.0000, 21.0551, 4.3107) },
		{ XMFLOAT3(-5.7727, 23.8732, 2.5201) },
		{ XMFLOAT3(0.0000, 21.1492, 10.4294) },
		{ XMFLOAT3(-3.0602 ,22.1858, 11.1401) },
		{ XMFLOAT3(0.0000, 49.8742, 32.4936) },
		{ XMFLOAT3(0.0000, 53.7564 ,33.7791) },
		{ XMFLOAT3(-4.2655,54.7130, 33.1236) },
		{ XMFLOAT3(-2.8472,48.9426 ,31.2480) },
		{ XMFLOAT3(-1.4717,61.2281 ,33.9047) },
		{ XMFLOAT3(-1.7216,62.2457 ,33.7362) },
		{ XMFLOAT3(-4.0187,61.5824, 32.8866) },
		{ XMFLOAT3(-4.0238,61.1644 ,33.0124) },
		{ XMFLOAT3(-4.5313,62.2139, 33.1023) },
		{ XMFLOAT3(-4.4531,60.0940 ,35.2123) },
		{ XMFLOAT3(-2.6558,59.6975,38.3042) },
		{ XMFLOAT3(-5.6666,62.9149, 31.4062) },
		{ XMFLOAT3(-5.1702,72.6362 ,31.9057) },
		{ XMFLOAT3(-4.5824,72.5148, 32.1162) },
		{ XMFLOAT3(-4.4256,72.7931 ,31.2641) },
		{ XMFLOAT3(-5.2705,72.9039,31.0672) },
		{ XMFLOAT3(-7.0413,63.7406, 28.1534) },
		{ XMFLOAT3(-7.1458,65.1197, 24.9639) },
		{ XMFLOAT3(-8.2859,62.8256, 23.6916) },
		{ XMFLOAT3(-8.3999,59.7912, 28.0147) },
		{ XMFLOAT3(-9.0858,58.9035, 21.6742) },
		{ XMFLOAT3(-7.4604,55.4711, 28.1595) },
		{ XMFLOAT3(-0.9293,58.3750, 41.3783) },
		{ XMFLOAT3(-2.8940,57.4168, 38.1060) },
		{ XMFLOAT3(-1.0943,56.7670, 40.4711) },
		{ XMFLOAT3(0.0000, 56.5808, 41.2973) },
		{ XMFLOAT3(0.0000 ,57.9318 ,42.0475) },
		{ XMFLOAT3(-6.3310, 49.3805, 26.4953) },
		{ XMFLOAT3(-7.9595 ,53.7720 ,18.6897) },
		{ XMFLOAT3(-10.1576, 47.3121 ,19.1146) },
		{ XMFLOAT3(-8.7778,45.5459 ,24.0163) },
		{ XMFLOAT3(-5.0685,60.3084 ,33.5396) },
		{ XMFLOAT3(-6.0319,58.6991 ,32.5019) },
		{ XMFLOAT3(-8.3835,36.2740, 1.8868) },
		{ XMFLOAT3(-8.3955,35.9091 ,-4.4930) },
		{ XMFLOAT3(-6.2355,46.9125, -3.7101) },
		{ XMFLOAT3(-7.7761,48.3654 ,8.0857) },
		{ XMFLOAT3(-5.1188,10.9534, 9.8226) },
		{ XMFLOAT3(-6.1522,9.4758 ,7.3635) },
		{ XMFLOAT3(-4.9467,23.6358, 18.0086) },
		{ XMFLOAT3(-8.5076,24.0759, 18.1242) },
		{ XMFLOAT3(-8.0895,11.9075, 13.4733) },
		{ XMFLOAT3(-6.1475,11.5406 ,13.3860) },
		{ XMFLOAT3(-10.8515 ,23.2784, 12.5962) },
		{ XMFLOAT3(-9.1764, 11.3333, 9.9591) },
		{ XMFLOAT3(-9.4023, 23.0664, 5.8743) },
		{ XMFLOAT3(-8.3340, 10.0490, 7.6081) },
		{ XMFLOAT3(-5.8983, 0.0000 ,11.4977) },
		{ XMFLOAT3(-4.9938, 2.2037 ,12.8056) },
		{ XMFLOAT3(-5.1279, 1.8671 ,18.3129) },
		{ XMFLOAT3(-4.8481, 0.0041 ,18.5810) },
		{ XMFLOAT3(-6.1620, 4.2247, 14.5227) },
		{ XMFLOAT3(-8.0264, 4.3028, 14.5425) },
		{ XMFLOAT3(-7.9759, 3.4445, 17.6800) },
		{ XMFLOAT3(-6.1507, 3.4345, 17.6784) },
		{ XMFLOAT3(-9.0650, 2.2037 ,12.8056) },
		{ XMFLOAT3(-8.9225, 1.8836, 18.3129) },
		{ XMFLOAT3(-8.1736, 0.0000 ,11.4977) },
		{ XMFLOAT3(-9.1968, 0.0041 ,18.5810) },
		{ XMFLOAT3(-8.9043, 35.0267, 22.8604) },
		{ XMFLOAT3(-5.2406, 34.5802, 25.1142) },
		{ XMFLOAT3(-11.4191 ,37.0898, 17.2978) },
		{ XMFLOAT3(0.0000 ,33.2126, 26.4552) },
		{ XMFLOAT3(2.8472, 48.9426 ,31.2480) },
		{ XMFLOAT3(-10.4593 ,37.0399, 7.6051) },
		{ XMFLOAT3(0.0000, 21.5596, 16.2258) },
		{ XMFLOAT3(-10.1035, 46.0860 ,12.8986) },
		{ XMFLOAT3(-2.0060 ,25.2460, -26.0957) },
		{ XMFLOAT3(-4.2080 ,32.5404, -35.3179) },
		{ XMFLOAT3(-4.3516 ,22.3232, -17.8305) },
		{ XMFLOAT3(-8.9037 ,38.8359, -32.4249) },
		{ XMFLOAT3(-8.3267 ,43.3950, -28.2470) },
		{ XMFLOAT3(-9.2743 ,42.4074, -19.7383) },
		{ XMFLOAT3(-10.3919, 34.5595, -23.0334) },
		{ XMFLOAT3(-8.9193 ,29.9965 ,-15.9450) },
		{ XMFLOAT3(-9.2631 ,38.8975, -13.6326) },
		{ XMFLOAT3(-8.4491 ,32.7561, -34.6817) },
		{ XMFLOAT3(-9.7046 ,26.9366, -26.1386) },
		{ XMFLOAT3(-8.3939 ,22.7467, -19.1145) },
		{ XMFLOAT3(-4.6150 ,19.2711, -34.4130) },
		{ XMFLOAT3(-3.2307 ,17.1211, -29.3654) },
		{ XMFLOAT3(-3.5255 ,9.9869 ,-35.4780) },
		{ XMFLOAT3(-4.6677 ,9.6743 ,-38.7066) },
		{ XMFLOAT3(-4.7730 ,16.1094, -25.3457) },
		{ XMFLOAT3(-4.6625 ,10.0819 ,-32.1364) },
		{ XMFLOAT3(-7.3679 ,19.3552, -34.2650) },
		{ XMFLOAT3(-6.8155 ,9.6812, -38.6720) },
		{ XMFLOAT3(-8.8503 ,17.3728, -29.4051) },
		{ XMFLOAT3(-8.1155 ,9.9978 ,-35.4814) },
		{ XMFLOAT3(-7.3837, 16.2034, -25.6691) },
		{ XMFLOAT3(-6.9462, 10.0864, -32.1445) },
		{ XMFLOAT3(-3.7241, 1.8910, -34.4107) },
		{ XMFLOAT3(-3.7681, 0.0033, -36.8421) },
		{ XMFLOAT3(-4.3549, 3.2387, -33.0995) },
		{ XMFLOAT3(-6.8771, 0.0000, -36.7865) },
		{ XMFLOAT3(-7.8598, 1.8910, -34.4107) },
		{ XMFLOAT3(-6.9602, 3.2387 ,-33.0995) },
		{ XMFLOAT3(-4.0291, 1.8128, -30.5165) },
		{ XMFLOAT3(-3.9159, 0.0000, -30.1141) },
		{ XMFLOAT3(-4.4474, 3.0088, -30.9967) },
		{ XMFLOAT3(-7.9766, -0.0000, -30.1141) },
		{ XMFLOAT3(-7.7557, 1.8411, -30.5165) },
		{ XMFLOAT3(-7.1373, 3.0088 ,-30.9967) },
		{ XMFLOAT3(-6.9155, 47.0415, -25.3037) },
		{ XMFLOAT3(-7.0557, 46.9134, -18.4973) },
		{ XMFLOAT3(-7.7411, 37.6322, -10.0544) },
		{ XMFLOAT3(0.0000 ,53.1964 ,-26.3909) },
		{ XMFLOAT3(-5.3563 ,53.1964 ,-29.7765) },
		{ XMFLOAT3(-4.7260 ,50.4054 ,-34.3469) },
		{ XMFLOAT3(0.0000, 50.4054, -35.7716) },
		{ XMFLOAT3(-5.6460, 57.4789, -29.5361) },
		{ XMFLOAT3(-4.9817 ,59.4355 ,-36.2693) },
		{ XMFLOAT3(0.0000, 60.0403, -37.6438) },
		{ XMFLOAT3(0.0000 ,56.0416, -26.2696) },
		{ XMFLOAT3(-5.6959, 60.7511 ,-26.1066) },
		{ XMFLOAT3(-5.0256 ,66.4289, -30.4633) },
		{ XMFLOAT3(0.0000 ,67.6308, -31.3855) },
		{ XMFLOAT3(0.0000 ,57.8949 ,-23.9149) },
		{ XMFLOAT3(-5.5917 ,61.4858 ,-21.0244) },
		{ XMFLOAT3(-4.9337 ,68.5099, -20.8712) },
		{ XMFLOAT3(0.0000, 69.9968 ,-20.8387) },
		{ XMFLOAT3(0.0000, 57.9523 ,-21.1015) },
		{ XMFLOAT3(-4.0168 ,59.3177 ,-16.4484) },
		{ XMFLOAT3(-3.5441, 62.5901, -12.6061) },
		{ XMFLOAT3(0.0000, 63.2828, -11.7927) },
		{ XMFLOAT3(0.0000 ,57.6715, -18.3813) },
		{ XMFLOAT3(-0.7100 ,54.1777 ,-15.3476) },
		{ XMFLOAT3(-0.6265, 54.3962 ,-14.5806) },
		{ XMFLOAT3(-0.0000, 54.2179, -14.2850) },
		{ XMFLOAT3(-0.0000 ,53.8865, -15.4218) },
		{ XMFLOAT3(-7.7813 ,45.2165, -13.8001) },
		{ XMFLOAT3(-6.5323 ,46.0398 ,-11.4981) },
		{ XMFLOAT3(-4.9625, 58.1897, 35.1297) },
		{ XMFLOAT3(0.0000 ,54.1479 ,39.9463) },
		{ XMFLOAT3(-1.3042 ,54.7211, 39.3169) },
		{ XMFLOAT3(-1.9991 ,54.6185, 37.9748) },
		{ XMFLOAT3(0.0000 ,54.0544 ,38.4723) },
		{ XMFLOAT3(-3.1945, 54.5084 ,35.5930) },
		{ XMFLOAT3(0.0000 ,53.8959 ,35.9760) },
		{ XMFLOAT3(1.0703, 60.0632 ,42.5839) },
		{ XMFLOAT3(1.2073, 61.7317 ,38.5023) },
		{ XMFLOAT3(1.4735 ,62.9793 ,35.3937) },
		{ XMFLOAT3(1.6151, 66.4783 ,33.5434) },
		{ XMFLOAT3(2.0337,68.0208 ,30.3775) },
		{ XMFLOAT3(1.4123,68.4196 ,25.0142) },
		{ XMFLOAT3(3.3688,66.7840 ,22.0988) },
		{ XMFLOAT3(4.9529,61.9067 ,17.5324) },
		{ XMFLOAT3(5.2209,58.7555 ,13.7239) },
		{ XMFLOAT3(5.4912,53.5690 ,7.0532) },
		{ XMFLOAT3(5.1013,50.5662 ,-4.6607) },
		{ XMFLOAT3(4.2878,49.8122 ,-16.8214) },
		{ XMFLOAT3(3.7827,49.8836 ,-26.7780) },
		{ XMFLOAT3(3.5104,46.1778 ,-31.3639) },
		{ XMFLOAT3(3.1590,39.3728 ,-33.9397) },
		{ XMFLOAT3(1.5250,31.9297 ,-23.0912) },
		{ XMFLOAT3(3.9890,28.8871 ,-12.8093) },
		{ XMFLOAT3(6.1336,26.9997 ,-4.1936) },
		{ XMFLOAT3(5.7727,23.8732 ,2.5201) },
		{ XMFLOAT3(3.0602,22.1858 ,11.1401) },
		{ XMFLOAT3(4.2655,54.7130 ,33.1236) },
		{ XMFLOAT3(1.4717,61.2281 ,33.9047) },
		{ XMFLOAT3(4.0238,61.1644 ,33.0124) },
		{ XMFLOAT3(4.0187,61.5824 ,32.8866) },
		{ XMFLOAT3(1.7216,62.2457 ,33.7362) },
		{ XMFLOAT3(4.5313, 62.2139 ,33.1023) },
		{ XMFLOAT3(2.6558, 59.6975 ,38.3042) },
		{ XMFLOAT3(4.4531, 60.0940 ,35.2123) },
		{ XMFLOAT3(5.6666, 62.9149 ,31.4062) },
		{ XMFLOAT3(5.1702, 72.6362 ,31.9057) },
		{ XMFLOAT3(5.2705, 72.9039 ,31.0672) },
		{ XMFLOAT3(4.4256, 72.7931 ,31.2641) },
		{ XMFLOAT3(4.5824, 72.5148 ,32.1162) },
		{ XMFLOAT3(7.0413, 63.7406 ,28.1534) },
		{ XMFLOAT3(7.1458, 65.1197 ,24.9639) },
		{ XMFLOAT3(8.3999, 59.7912 ,28.0147) },
		{ XMFLOAT3(8.2859, 62.8256 ,23.6916) },
		{ XMFLOAT3(7.4604, 55.4711 ,28.1595) },
		{ XMFLOAT3(9.0858, 58.9035 ,21.6742) },
		{ XMFLOAT3(0.9293, 58.3750 ,41.3783) },
		{ XMFLOAT3(1.0943, 56.7670 ,40.4711) },
		{ XMFLOAT3(2.8940, 57.4168 ,38.1060) },
		{ XMFLOAT3(6.3310, 49.3805 ,26.4953) },
		{ XMFLOAT3(8.7778, 45.5459, 24.0163) },
		{ XMFLOAT3(10.1576 ,47.3121, 19.1146) },
		{ XMFLOAT3(7.9595, 53.7720, 18.6897) },
		{ XMFLOAT3(5.0685, 60.3084 ,33.5396) },
		{ XMFLOAT3(6.0319 ,58.6991, 32.5019) },
		{ XMFLOAT3(8.3955 ,35.9091, -4.4930) },
		{ XMFLOAT3(8.3835 ,36.2740, 1.8868) },
		{ XMFLOAT3(7.7761 ,48.3654 ,8.0857) },
		{ XMFLOAT3(6.2355 ,46.9125, -3.7101) },
		{ XMFLOAT3(6.1522 ,9.4758 ,7.3635) },
		{ XMFLOAT3(5.1188 ,10.9534,9.8226) },
		{ XMFLOAT3(4.9467 ,23.6358,18.0086) },
		{ XMFLOAT3(6.1475 ,11.5406,13.3860) },
		{ XMFLOAT3(8.0895 ,11.9075,13.4733) },
		{ XMFLOAT3(8.5076 ,24.0759,18.1242) },
		{ XMFLOAT3(9.1764 ,11.3333,9.9591) },
		{ XMFLOAT3(10.8515 ,23.2784 ,12.5962) },
		{ XMFLOAT3(9.4023 ,23.0664, 5.8743) },
		{ XMFLOAT3(8.3340 ,10.0490, 7.6081) },
		{ XMFLOAT3(5.8983 ,0.0000, 11.4977) },
		{ XMFLOAT3(4.8481 ,0.0041,18.5810) },
		{ XMFLOAT3(5.1279 ,1.8671,18.3129) },
		{ XMFLOAT3(4.9938 ,2.2037,12.8056) },
		{ XMFLOAT3(6.1620 ,4.2247,14.5227) },
		{ XMFLOAT3(6.1507 ,3.4345,17.6784) },
		{ XMFLOAT3(7.9759 ,3.4445,17.6800) },
		{ XMFLOAT3(8.0264 ,4.3028,14.5425) },
		{ XMFLOAT3(8.9225 ,1.8836,18.3129) },
		{ XMFLOAT3(9.0650 ,2.2037,12.8056) },
		{ XMFLOAT3(8.1736 ,0.0000,11.4977) },
		{ XMFLOAT3(9.1968 ,0.0041,18.5810) },
		{ XMFLOAT3(8.9043 ,35.0267, 22.8604) },
		{ XMFLOAT3(5.2406 ,34.5802 ,25.1142) },
		{ XMFLOAT3(11.4191, 37.0898, 17.2978) },
		{ XMFLOAT3(10.4593, 37.0399, 7.6051) },
		{ XMFLOAT3(10.1035, 46.0860 ,12.8986) },
		{ XMFLOAT3(4.2080 ,32.5404, -35.3179) },
		{ XMFLOAT3(2.0060 ,25.2460, -26.0957) },
		{ XMFLOAT3(4.3516 ,22.3232, -17.8305) },
		{ XMFLOAT3(8.3267 ,43.3950, -28.2470) },
		{ XMFLOAT3(8.9037 ,38.8359, -32.4249) },
		{ XMFLOAT3(9.2743 ,42.4074, -19.7383) },
		{ XMFLOAT3(9.2631 ,38.8975, -13.6326) },
		{ XMFLOAT3(8.9193 ,29.9965, -15.9450) },
		{ XMFLOAT3(10.3919, 34.5595, -23.0334) },
		{ XMFLOAT3(8.4491 ,32.7561, -34.6817) },
		{ XMFLOAT3(9.7046 ,26.9366, -26.1386) },
		{ XMFLOAT3(8.3939 ,22.7467, -19.1145) },
		{ XMFLOAT3(4.6150 ,19.2711, -34.4130) },
		{ XMFLOAT3(4.6677 ,9.6743, -38.7066) },
		{ XMFLOAT3(3.5255 ,9.9869, -35.4780) },
		{ XMFLOAT3(3.2307 ,17.1211, -29.3654) },
		{ XMFLOAT3(4.6625 ,10.0819, -32.1364) },
		{ XMFLOAT3(4.7730 ,16.1094, -25.3457) },
		{ XMFLOAT3(7.3679 ,19.3552, -34.2650) },
		{ XMFLOAT3(6.8155 ,9.6812, -38.6720) },
		{ XMFLOAT3(8.8503 ,17.3728, -29.4051) },
		{ XMFLOAT3(8.1155 ,9.9978, -35.4814) },
		{ XMFLOAT3(7.3837 ,16.2034 ,-25.6691) },
		{ XMFLOAT3(6.9462 ,10.0864, -32.1445) },
		{ XMFLOAT3(3.7681 ,0.0033, -36.8421) },
		{ XMFLOAT3(3.7241 ,1.8910, -34.4107) },
		{ XMFLOAT3(4.3549 ,3.2387 ,-33.0995) },
		{ XMFLOAT3(6.8771 ,0.0000, -36.7865) },
		{ XMFLOAT3(7.8598 ,1.8910, -34.4107) },
		{ XMFLOAT3(6.9602 ,3.2387 ,-33.0995) },
		{ XMFLOAT3(3.9159 ,0.0000, -30.1141) },
		{ XMFLOAT3(4.0291 ,1.8128, -30.5165) },
		{ XMFLOAT3(4.4474 ,3.0088 ,-30.9967) },
		{ XMFLOAT3(7.9766 ,-0.0000, -30.1141) },
		{ XMFLOAT3(7.7557 ,1.8411, -30.5165) },
		{ XMFLOAT3(7.1373 ,3.0088, -30.9967) },
		{ XMFLOAT3(6.9155 ,47.0415, -25.3037) },
		{ XMFLOAT3(7.0557 ,46.9134, -18.4973) },
		{ XMFLOAT3(7.7411 ,37.6322,-10.0544) },
		{ XMFLOAT3(5.3563 ,53.1964,-29.7765) },
		{ XMFLOAT3(4.7260 ,50.4054,-34.3469) },
		{ XMFLOAT3(4.9817 ,59.4355,-36.2693) },
		{ XMFLOAT3(5.6460 ,57.4789,-29.5361) },
		{ XMFLOAT3(5.0256 ,66.4289,-30.4633) },
		{ XMFLOAT3(5.6959 ,60.7511,-26.1066) },
		{ XMFLOAT3(4.9337 ,68.5099,-20.8712) },
		{ XMFLOAT3(5.5917 ,61.4858,-21.0244) },
		{ XMFLOAT3(3.5441 ,62.5901,-12.6061) },
		{ XMFLOAT3(4.0168 ,59.3177,-16.4484) },
		{ XMFLOAT3(0.6265 ,54.3962,-14.5806) },
		{ XMFLOAT3(0.7100 ,54.1777,-15.3476) },
		{ XMFLOAT3(7.7813 ,45.2165,-13.8001) },
		{ XMFLOAT3(6.5323 ,46.0398,-11.4981) },
		{ XMFLOAT3(4.9625 ,58.1897 ,35.1297) },
		{ XMFLOAT3(1.9991 ,54.6185 ,37.9748) },
		{ XMFLOAT3(1.3042 ,54.7211, 39.3169) },
		{ XMFLOAT3(3.1945 ,54.5084 ,35.5930) },
	
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
		static unsigned int quadIndices[] = {

		102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		/* body */
	12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 ,
	 24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40 ,
	 /* lid */
  96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,
  0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117 ,
  /* bottom */
118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37 ,
/* handle */
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56 ,
53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67 ,
/* spout */
68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83 ,
80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95
		};

		m_indexCount = ARRAYSIZE(quadIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = quadIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC indexBufferDescription(sizeof(quadIndices), D3D11_BIND_INDEX_BUFFER);

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDescription, &indexBufferData, &m_indexBuffer));

		});

	CD3D11_RASTERIZER_DESC rasterStateDesc(D3D11_DEFAULT);

	rasterStateDesc.CullMode = D3D11_CULL_NONE;
	rasterStateDesc.FillMode = D3D11_FILL_WIREFRAME;

	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateRasterizerState(
			&rasterStateDesc,
			m_rasterizerState.GetAddressOf()
		)
	);

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {
		m_loadingComplete = true;
		});
}

void Dog::SetViewProjectionMatrixConstantBuffer(DirectX::XMMATRIX& view, DirectX::XMMATRIX& projection)
{
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.view, DirectX::XMMatrixTranspose(view));

	DirectX::XMStoreFloat4x4(&m_MVPBufferData.projection, DirectX::XMMatrixTranspose(projection));

}

void Dog::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_hullShader.Reset();
	m_domainShader.Reset();
	m_pixelShader.Reset();
	m_MVPBuffer.Reset();
	m_timeBuffer.Reset();
	m_cameraBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}
void Dog::SetCameraPositionConstantBuffer(DirectX::XMFLOAT3& cameraPosition)
{
	m_cameraBufferData.position = cameraPosition;
	m_cameraBufferData.padding = 1;
}

void Dog::Update(DX::StepTimer const& timer)
{
	auto worldMatrix = DirectX::XMMatrixIdentity();
	////m_rotation.x +=timer.GetElapsedSeconds() * -0.2f;
	//m_rotation.y += timer.GetElapsedSeconds() * 0.2f;
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z));
	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z)));
	//worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixRotationY(radians));

	worldMatrix = XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));

	//m_timeBufferData.time = timer.GetTotalSeconds();

	//DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, worldMatrix)));
	DirectX::XMStoreFloat4x4(&m_MVPBufferData.model, DirectX::XMMatrixTranspose(worldMatrix));
}

void Dog::Render()
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
	//context->RSSetState(m_rasterizerState.Get());
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);
	context->VSSetConstantBuffers1(
		0,
		1,
		m_MVPBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);
	////// Attach our vertex shader.
	context->DSSetShader(
		m_domainShader.Get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
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
		m_cameraBuffer.GetAddressOf(),
		nullptr,
		nullptr
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);
	context->HSSetShader(m_hullShader.Get(), nullptr, 0);

	context->GSSetShader(
		nullptr,
		nullptr,
		0
	);

	// Draw the objects.
		// Draw the objects.
	context->DrawIndexed(
		m_indexCount,
		0,0

	);

}

