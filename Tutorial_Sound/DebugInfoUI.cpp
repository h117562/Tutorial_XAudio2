#include "DebugInfoUI.h"

DebugInfoUI::DebugInfoUI()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = 0;
}

bool DebugInfoUI::Initialize(D3DClass* d3dClass)
{
	bool result;
	char videoCard[128];
	int videoMemory;

	//그래픽 카드 이름과, 메모리 크기를 가져옴
	d3dClass->GetVideoCardInfo(videoCard, videoMemory);

	//변수 초기화
	wcscpy_s(m_videoNameString, L"");
	MultiByteToWideChar(CP_ACP, 0, videoCard, strlen(videoCard) + 1, m_videoNameString, _countof(m_videoNameString));
	//wchar_t용 Itoa, 마지막 매개변수는 10진수를 뜻함 기본 16진수이므로 꼭 10 써줘야 함
	_itow_s(videoMemory, m_tempString, _countof(m_tempString), 10); 

	wcscpy_s(m_memoryString, L"VRAM: ");
	wcscat_s(m_memoryString, m_tempString);
	wcscat_s(m_memoryString, L" MB");

	//변수 초기화
	m_previousPos = XMINT3(0, 0, 0);
	m_previousRot = XMINT3(0, 0, 0);

	wcscpy_s(m_stringPX, L"PX: 0");
	wcscpy_s(m_stringPY, L"PY: 0");
	wcscpy_s(m_stringPZ, L"PZ: 0");

	wcscpy_s(m_stringRX, L"RX: 0");
	wcscpy_s(m_stringRY, L"RY: 0");
	wcscpy_s(m_stringRZ, L"RZ: 0");

	return true;
}

void DebugInfoUI::Render(TextClass* ptextclass, CameraClass* pCameraClass)
{
	XMFLOAT3 position, rotation;

	pCameraClass->GetPosition(position);
	pCameraClass->GetRotation(rotation);

	m_position = XMINT3((int)position.x, (int)position.y, (int)position.z);
	m_rotation = XMINT3((int)rotation.x, (int)rotation.y, (int)rotation.z);

	GetFps();

	//Fps를 출력
	wcscpy_s(m_fpsString, L"FPS: ");
	_itow_s(m_fps, m_tempString, _countof(m_tempString), 10);
	wcscat_s(m_fpsString, m_tempString);
	ptextclass->RenderText(m_fpsString, 0, 0, 800, 400);


	//비디오 카드 정보를 출력
	ptextclass->RenderText(m_videoNameString, 0, 30, 800, 400);
	ptextclass->RenderText(m_memoryString, 0, 60, 800, 400);


	//값 변동이 있을 경우에만 텍스트 초기화
	if (m_position.x != m_previousPos.x)
	{
		wcscpy_s(m_stringPX, L"PX: ");
		_itow_s(m_position.x, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringPX, m_tempString);
		m_previousPos.x = m_position.x;
	}

	if (m_position.y != m_previousPos.y)
	{
		wcscpy_s(m_stringPY, L"PY: ");
		_itow_s(m_position.y, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringPY, m_tempString);
		m_previousPos.y = m_position.y;
	}

	if (m_position.z != m_previousPos.z)
	{
		wcscpy_s(m_stringPZ, L"PZ: ");
		_itow_s(m_position.z, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringPZ, m_tempString);
		m_previousPos.z = m_position.z;
	}

	if (m_rotation.x != m_previousRot.x)
	{
		wcscpy_s(m_stringRX, L"RX: ");
		_itow_s(m_rotation.x, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringRX, m_tempString);
		m_rotation.x = m_previousRot.x;
	}

	if (m_rotation.y != m_previousRot.y)
	{
		wcscpy_s(m_stringRY, L"RY: ");
		_itow_s(m_rotation.y, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringRY, m_tempString);
		m_rotation.y = m_previousRot.y;
	}

	if (m_rotation.z != m_previousRot.z)
	{
		wcscpy_s(m_stringRZ, L"RZ: ");
		_itow_s(m_rotation.z, m_tempString, _countof(m_tempString), 10);
		wcscat_s(m_stringRZ, m_tempString);
		m_rotation.z = m_previousRot.z;
	}

	//카메라 위치 정보를 출력
	ptextclass->RenderText(m_stringPX, 0, 120, 800, 400);
	ptextclass->RenderText(m_stringPY, 0, 150, 800, 400);
	ptextclass->RenderText(m_stringPZ, 0, 180, 800, 400);
	ptextclass->RenderText(m_stringRX, 0, 210, 800, 400);
	ptextclass->RenderText(m_stringRY, 0, 240, 800, 400);
	ptextclass->RenderText(m_stringRZ, 0, 270, 800, 400);


	return;
}

void DebugInfoUI::GetFps()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}

	return;
}

void DebugInfoUI::Shutdown()
{
	return;
}