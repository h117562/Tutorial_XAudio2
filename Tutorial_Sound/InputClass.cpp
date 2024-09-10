#include "InputClass.h"

InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboardDevice = 0;
	m_mouseDevice = 0;
	m_sensitivity = 0.1f;

	ZeroMemory(m_currKeyState, sizeof(m_currKeyState));
	ZeroMemory(m_prevKeyState, sizeof(m_prevKeyState));
	ZeroMemory(m_prevMouseState, sizeof(m_prevMouseState));
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result;

	//DirectInput 생성
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//키보드용 Device 생성
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboardDevice, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//데이터 형식을 키보드 형식으로 설정
	result = m_keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}

	//협력 수준 설정
	result = m_keyboardDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	//키보드 장치 획득
	result = m_keyboardDevice->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	//마우스용 Device 생성
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouseDevice, nullptr);
	if (FAILED(result))
	{
		return false;
	}

	//데이터 형식을 마우스 형식으로 설정
	result = m_mouseDevice->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(result))
	{
		return false;
	}

	//협력 수준 설정
	result = m_mouseDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}

	//마우스 장치 획득
	result = m_mouseDevice->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool InputClass::Frame()
{
	bool result;

	//마우스 State를 업데이트
	result = ReadMouse();
	if (!result)
	{
		return false;
	}

	//키보드 State를 업데이트
	result = ReadKeyboard();
	if (!result)
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	//마우스 DirectDevice 해제
	if (m_mouseDevice)
	{
		m_mouseDevice->Unacquire();
		m_mouseDevice->Release();
		m_mouseDevice = NULL;
	}

	//키보드 DirectDevice 해제
	if (m_keyboardDevice)
	{
		m_keyboardDevice->Unacquire();
		m_keyboardDevice->Release();
		m_keyboardDevice = NULL;
	}

	//DirectInput 해제
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = NULL;
	}

	return;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	//키보드 장치의 상태를 가져옴
	result = m_keyboardDevice->GetDeviceState(sizeof(m_currKeyState), (LPVOID)&m_currKeyState);
	if (FAILED(result))
	{
		//DirectInputDevice에 문제가 생겼을 경우 장치를 다시 받아오는 부분
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboardDevice->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	//마우스 장치의 상태를 가져옴
	result = m_mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{

		//DirectInputDevice에 문제가 생겼을 경우 장치를 다시 받아오는 부분
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouseDevice->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

//마우스의 좌표를 얻는 함수
void InputClass::GetMousePosition(long& posX, long& posY)
{
	POINT pos;

	GetCursorPos(&pos);

	posX = pos.x;
	posY = pos.y;

	return;
}

//정규화된 마우스의 좌표를 얻는 함수
void InputClass::GetNormalizedMousePosition(float& posX, float& posY)
{
	return;
}

//마우스의 움직임을 얻는 함수
void InputClass::GetMouseDelta(float& posX, float& posY)
{
	posX = m_mouseState.lX * m_sensitivity;
	posY = m_mouseState.lY * m_sensitivity;

	return;
}

//키가 눌리면 true를 리턴하는 함수
bool InputClass::GetKeyPressed(const unsigned char keyCode)
{
	if (KEYDOWN(m_currKeyState, keyCode))
	{
		return true;
	}

	return false;
}

//키가 눌렸다가 땟을 때 true를 리턴하는 함수
bool InputClass::GetKeyPressedAndRelease(const unsigned char keyCode)
{
	if (!KEYDOWN(m_currKeyState, keyCode))
	{
		if (KEYDOWN(m_prevKeyState, keyCode))
		{
			m_prevKeyState[keyCode] = 0x00;
			return true;
		}
	}
	else
	{
		m_prevKeyState[keyCode] = 0x80;
	}

	return false;
}

//키가 땟다가 눌렸을 때 true를 리턴하는 함수
bool InputClass::GetKeyReleasedAndPress(const unsigned char keyCode)
{
	if (KEYDOWN(m_currKeyState, keyCode))
	{
		if (!KEYDOWN(m_prevKeyState, keyCode))
		{
			m_prevKeyState[keyCode] = 0x80;
			return true;
		}
	}
	else
	{
		m_prevKeyState[keyCode] = 0x00;
	}

	return false;
}

//마우스 왼쪽 버튼이 눌렸는지 체크하는 함수
bool InputClass::GetLeftMouseButtonDown()
{
	if (m_mouseState.rgbButtons[0] & 0x80)
	{
		return true;
	}
	
	return false;
}

//마우스 오른쪽 버튼이 눌렸는지 체크하는 함수
bool InputClass::GetRightMouseButtonDown()
{
	if (m_mouseState.rgbButtons[1] & 0x80)
	{
		return true;
	}

	return false;
}

//마우스 왼쪽 버튼이 눌렸다 떗는지 체크하는 함수
bool InputClass::GetLeftMouseButtonUp()
{
	if (!KEYDOWN(m_mouseState.rgbButtons, 0))
	{
		if (m_prevMouseState[0])
		{
			m_prevMouseState[0] = false;
			return true;
		}
	}
	else
	{
		m_prevMouseState[0] = true;
	}

	return false;
}

//마우스 오른쪽 버튼이 눌렸다 떗는지 체크하는 함수
bool InputClass::GetRightMouseButtonUp()
{
	if (!KEYDOWN(m_mouseState.rgbButtons, 1))
	{
		if (m_prevMouseState[1])
		{
			m_prevMouseState[1] = false;
			return true;
		}
	}
	else
	{
		m_prevMouseState[1] = true;
	}

	return false;
}