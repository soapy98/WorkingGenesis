class Camera
{
public:
	Camera();
	~Camera();
	const DirectX::XMMATRIX& GetProjectionMatrix() const;
	const DirectX::XMMATRIX& GetViewMatrix() const;
	void UpdateViewMatrix();
	void addPitch(float in);
	void addYaw(float in);

	void AdjustPosition(const float x, const float y, const float z);
	void SetLookAtPos(DirectX::XMFLOAT3& lookAtPos);
	void panVerticle(float in);

	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	void SetPosition(const DirectX::XMVECTOR& inPos);


	const DirectX::XMFLOAT3& GetPositionFloat3() const;

	const DirectX::XMVECTOR& GetPosVector() const;
private:
	
	float pitch, yaw,roll;
	DirectX::XMVECTOR defaultRight = DirectX::XMVectorSet(1, 0, 0, 0);
	DirectX::XMVECTOR defaultForward = DirectX::XMVectorSet(0, 0, 1, 0);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR Right = DirectX::XMVectorSet(1, 0, 0, 0);
	DirectX::XMVECTOR Forward = DirectX::XMVectorSet(0, 0, 1, 0);
	DirectX::XMVECTOR target = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR pos = DirectX::XMVectorSet(0, 0, 0, 0);
	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMFLOAT3 m_pos;
};	



