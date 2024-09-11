#include "Systemclass.h"


SystemClass::SystemClass()
{
	m_applicationClass = 0;
	m_inputClass = 0;
	m_frameTimer = 0;
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int screenwidth, screenheight;
	int posX, posY;
	bool result;

	//WndProc 함수에서 SystemClass의 인스턴스를 참조하기 위한 전역 변수
	ApplicationHandle = this;

	//현재 프로그램의 프로세스 인스턴스 핸들을 가져옴
	m_hinstance = GetModuleHandle(NULL);

	//윈도우 클래스 이름
	m_applicationName = L"Tutorial Sound";

	//윈도우 클래스 설정
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//윈도우 클래스 등록
	RegisterClassEx(&wc);

	//스크린크기
	screenwidth = 1200;
	screenheight = 800;

	//화면 중앙에 정렬
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenwidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenheight) / 2;


	//프로그램 윈도우 생성
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_OVERLAPPEDWINDOW,
		posX, posY, screenwidth, screenheight, NULL, NULL, m_hinstance, NULL);

	//윈도우 포커스 가져오기
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	
	m_frameTimer = new FrameTimer();
	if (!m_frameTimer)
	{
		return false;
	}

	//FrameTimer 구현
	result = m_frameTimer->Initialize();
	if (!result)
	{
		return false;
	}

	m_inputClass = new InputClass();
	if (!m_inputClass)
	{
		return false;
	}

	//InputClass 구현
	result = m_inputClass->Initialize(m_hinstance, m_hwnd);
	if (!result)
	{
		return false;
	}

	m_applicationClass = new ApplicationClass();
	if (!m_applicationClass)
	{
		return false;
	}

	result = m_applicationClass->Initialize(m_hinstance, m_hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR, m_inputClass);
	if (!result)
	{
		return false;
	}

	return true;
}


void SystemClass::Shutdown()
{

	if (m_applicationClass)
	{
		m_applicationClass->Shutdown();
		delete m_applicationClass;
		m_applicationClass = 0;
	}

	if (m_inputClass)
	{
		m_inputClass->Shutdown();
		delete m_inputClass;
		m_inputClass = 0;
	}

	if (m_frameTimer)
	{
		delete m_frameTimer;
		m_frameTimer = 0;
	}

	//윈도우 핸들 파괴
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//등록된 윈도우 클래스를 해제
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//SystemClass 핸들 포인터를 NULL로 초기화
	ApplicationHandle = NULL;

	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	//변수 초기화
	ZeroMemory(&msg, sizeof(MSG));


	done = false;
	while (!done)
	{
		//메시지 전달확인
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//종료 메시지 확인
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//종료가 아니면 사이클반복
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	m_frameTimer->Frame();

	result = m_inputClass->Frame();
	if (!result)
	{
		return false;
	}

	result = m_applicationClass->Frame(m_hwnd, m_inputClass, m_frameTimer);
	if (!result)
	{
		return false;
	}

	return true;
}


LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}