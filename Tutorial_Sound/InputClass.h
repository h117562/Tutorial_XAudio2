#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#define KEYDOWN(keyState, keyCode) (keyState[keyCode] & 0x80) 

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class InputClass
{
public:
	InputClass();
	~InputClass();

	bool Initialize(HINSTANCE, HWND);
	bool Frame();
	void Shutdown();

	bool GetKeyPressed(const unsigned char keyCode);
	bool GetKeyPressedAndRelease(const unsigned char keyCode);
	bool GetKeyReleasedAndPress(const unsigned char keyCode);

	bool GetLeftMouseButtonDown();
	bool GetRightMouseButtonDown();
	bool GetLeftMouseButtonUp();
	bool GetRightMouseButtonUp();

	void GetMousePosition(long&, long&);
	void GetNormalizedMousePosition(float&, float&);
	void GetMouseDelta(float&, float&);

private:
	bool ReadKeyboard();
	bool ReadMouse();

private:
	LPDIRECTINPUT8 m_directInput;
	LPDIRECTINPUTDEVICE8 m_keyboardDevice, m_mouseDevice;
	DIMOUSESTATE2 m_mouseState;
	unsigned char m_currKeyState[256];
	unsigned char m_prevKeyState[256];

	bool m_prevMouseState[8];
	float m_sensitivity;
};

#endif
