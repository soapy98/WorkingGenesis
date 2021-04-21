#include "pch.h"
#include "Camera.h"
#include <iostream>
using namespace DirectX;
Camera::Camera() : pitch(), yaw(), m_pos(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	this->pos = XMLoadFloat3(&m_pos);
}


Camera::~Camera()
{
}


const XMMATRIX& Camera::GetViewMatrix() const
{
	return XMMatrixLookAtRH(pos, target, up);
}

void Camera::UpdateViewMatrix()
{
	const	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, 0);
	target = XMVector3TransformCoord(defaultForward, camRotationMatrix);
	target = XMVector3Normalize(target);

	Right = XMVector3TransformNormal(defaultRight, camRotationMatrix);
	Forward = XMVector3TransformNormal(defaultForward, camRotationMatrix);
	up = XMVector3Cross(Forward, Right);
	target += pos;
	XMLoadFloat3(&m_pos) = pos;

}

void Camera::addPitch(const float in)
{
	pitch += in;
}

void Camera::addYaw(const float in)
{
	yaw += in;
}


void Camera::AdjustPosition(const float x, const float y, const float z)
{

	this->m_pos.x += x;
	this->m_pos.y += y;
	this->m_pos.z += z;

	const XMVECTOR xV = Right * x;
	const	XMVECTOR yV = up * y;
	const	XMVECTOR zV = Forward * z;

	this->pos += xV;
	this->pos += yV;
	this->pos += zV;
}


void Camera::SetLookAtPos(XMFLOAT3& lookAtPos)
{
	//Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
	if (abs(round(lookAtPos.x)) == abs(round(this->m_pos.x)) && abs(round(lookAtPos.y)) == abs(round(this->m_pos.y)) && abs(round(lookAtPos.z)) == abs(round(this->m_pos.z)))
		return;

	lookAtPos.x = this->m_pos.x - lookAtPos.x;
	lookAtPos.y = this->m_pos.y - lookAtPos.y;
	lookAtPos.z = this->m_pos.z - lookAtPos.z;

	float pitc = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitc = atan(lookAtPos.y / distance);
	}

	float ya = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		ya = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		ya += XM_PI;
	pitch = pitc;
	yaw = ya;
}

void Camera::panVerticle(const float in)
{
	pos += up * in;
}
const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}
void Camera::SetProjectionValues(const float fovDegrees, const float aspectRatio, const float nearZ, const float farZ)
{
	this->projectionMatrix = XMMatrixPerspectiveFovRH(fovDegrees, aspectRatio, nearZ, farZ);
}
void Camera::SetPosition(const XMVECTOR& inpos)
{
	XMStoreFloat3(&this->m_pos, inpos);
	pos = inpos;
}
const XMFLOAT3& Camera::GetPositionFloat3()const
{
	return	m_pos;
}
const XMVECTOR& Camera::GetPosVector()const
{
	return pos;
}
