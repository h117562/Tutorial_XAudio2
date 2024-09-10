#include "CameraClass.h"


CameraClass::CameraClass()
{
	m_leftSpeed = 0;
	m_rightSpeed = 0;
	m_forwardSpeed = 0;
	m_backwardSpeed = 0;

	m_frameTime = 0;
	m_maxSpeed = 10.0f;

	m_position = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_upVector = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_lookAtVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	Render();//m_viewMatrix 초기화
	m_baseViewMatrix = m_viewMatrix;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
	return;
}


void CameraClass::SetRotation(DirectX::XMFLOAT3 rotation)
{
	m_rotation = rotation;
	return;
}


void CameraClass::GetPosition(DirectX::XMFLOAT3& position)
{
	position = m_position;
	return;
}


void CameraClass::GetRotation(DirectX::XMFLOAT3& rotation)
{
	rotation = m_rotation;
	return;
}

void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void CameraClass::GetBaseViewMatrix(DirectX::XMMATRIX& baseViewMatrix)
{
	baseViewMatrix = m_baseViewMatrix;
	return;
}

void CameraClass::SetFrameTime(const float time)
{
	m_frameTime = time;
	return;
}


void CameraClass::Render()
{
	DirectX::XMVECTOR upVector, positionVector, lookAtVector;
	DirectX::XMMATRIX rotationMatrix;
	float yaw, pitch, roll;
	
	positionVector = DirectX::XMLoadFloat3(&m_position);

	pitch = DirectX::XMConvertToRadians(m_rotation.x);
	yaw = DirectX::XMConvertToRadians(m_rotation.y);
	roll = DirectX::XMConvertToRadians(m_rotation.z);

	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	lookAtVector = DirectX::XMVector3TransformCoord(m_lookAtVector, rotationMatrix);
	upVector = DirectX::XMVector3TransformCoord(m_upVector, rotationMatrix);
	lookAtVector = DirectX::XMVectorAdd(positionVector, lookAtVector);
	m_viewMatrix = DirectX::XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}



void CameraClass::MoveForward(bool keyDown)
{
	float radians;

	if (keyDown)
	{
		m_forwardSpeed += m_frameTime;
		if (m_forwardSpeed > (m_frameTime * m_maxSpeed))
		{
			m_forwardSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime;

		if (m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radians = m_rotation.y * 0.0174532925f;

	// Update the position.
	m_position.x += sinf(radians) * m_forwardSpeed;
	m_position.z += cosf(radians) * m_forwardSpeed;

}

void CameraClass::MoveBackward(bool keyDown)
{
	float radians;


	
	if (keyDown)
	{
		m_backwardSpeed += m_frameTime;

		if (m_backwardSpeed > (m_frameTime * m_maxSpeed))
		{
			m_backwardSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime;

		if (m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}


	radians = m_rotation.y * 0.0174532925f;

	// Update the position.
	m_position.x -= sinf(radians) * m_backwardSpeed;
	m_position.z -= cosf(radians) * m_backwardSpeed;


	return;
}


void CameraClass::MoveLeft(bool keyDown)
{
	float radians;

	if (keyDown)
	{
		m_leftSpeed += m_frameTime;

		if (m_leftSpeed > (m_frameTime * m_maxSpeed))
		{
			m_leftSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_leftSpeed -= m_frameTime;

		if (m_leftSpeed < 0.0f)
		{
			m_leftSpeed = 0.0f;
		}
	}


	radians = m_rotation.y * 0.0174532925f;

	m_position.x -= cosf(radians) * m_leftSpeed;
	m_position.z += sinf(radians) * m_leftSpeed;


	return;

}

void CameraClass::MoveRight(bool keyDown)
{
	float radians;

	if (keyDown)
	{
		m_rightSpeed += m_frameTime;

		if (m_rightSpeed > (m_frameTime * m_maxSpeed))
		{
			m_rightSpeed = m_frameTime * m_maxSpeed;
		}
	}
	else
	{
		m_rightSpeed -= m_frameTime;

		if (m_rightSpeed < 0.0f)
		{
			m_rightSpeed = 0.0f;
		}
	}

	radians = m_rotation.y * 0.0174532925f;

	m_position.x += cosf(radians) * m_rightSpeed;
	m_position.z -= sinf(radians) * m_rightSpeed;

}

void CameraClass::UpdateRotation(const float deltaX, const float deltaY)
{
	m_rotation.x += deltaY;
	m_rotation.y += deltaX;

	//카메라 회전 범위 제한
	ClampRotation();
}

//rotation가 초과한 경우 일정 범위로 제한
void CameraClass::ClampRotation()
{
	if (m_rotation.x < -90.0f)
	{
		m_rotation.x = -90.0f;
	}
	else if (m_rotation.x > 90.0f)
	{
		m_rotation.x = 90.0f;
	}

	if (m_rotation.y < 0.0f)
	{
		m_rotation.y += 360.0f;
	}
	else if (m_rotation.y > 360.0f)
	{
		m_rotation.y -= 360.0f;
	}
}