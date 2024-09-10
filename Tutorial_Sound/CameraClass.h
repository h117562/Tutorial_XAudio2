#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <directxmath.h>

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(DirectX::XMFLOAT3);
	void SetRotation(DirectX::XMFLOAT3);
	void SetFrameTime(const float);

	void GetPosition(DirectX::XMFLOAT3&);
	void GetRotation(DirectX::XMFLOAT3&);

	void Render();
	void GetViewMatrix(DirectX::XMMATRIX&);
	void GetBaseViewMatrix(DirectX::XMMATRIX&);

	void MoveForward(bool);
	void MoveBackward(bool);
	void MoveLeft(bool);
	void MoveRight(bool);

	void UpdateRotation(const float, const float);

private:
	void ClampRotation();

private:
	DirectX::XMFLOAT3 m_position, m_rotation;
	DirectX::XMMATRIX m_viewMatrix, m_baseViewMatrix;

	DirectX::XMVECTOR m_upVector, m_lookAtVector;

	float m_frameTime;

	float m_leftSpeed, m_rightSpeed;
	float m_forwardSpeed, m_backwardSpeed;

	float m_maxSpeed;
};

#endif