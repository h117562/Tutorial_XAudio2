#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_CameraClass = 0;
	m_uiManager = 0;
	m_TextClass = 0;
	m_SoundClass = 0;
	m_ShaderManager = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, bool vsyncEnabled, bool fullScreen, float screenDepth, float screenNear, InputClass* pInputClass)
{
	bool result;
	
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	result = m_Direct3D->Initialize(vsyncEnabled, hwnd, fullScreen, screenDepth, screenNear);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	m_ShaderManager = new ShaderManager;
	if (!m_ShaderManager)
	{
		return false;
	}

	result = m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ShaderManager.", L"Error", MB_OK);
		return false;
	}

	m_TextClass = new TextClass;
	if (!m_TextClass)
	{
		return false;
	}

	result = m_TextClass->Initialize(m_Direct3D->GetSwapChain());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextClass.", L"Error", MB_OK);
		return false;
	}

	m_uiManager = new InterfaceManager;
	if (!m_uiManager)
	{
		return false;
	}

	result = m_uiManager->Initialize(m_Direct3D);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the UI Manager Class.", L"Error", MB_OK);
		return false;
	}

	m_CameraClass = new CameraClass;
	if (!m_CameraClass)
	{
		return false;
	}

	m_SoundClass = new SoundClass;
	if (!m_SoundClass)
	{
		return false;
	}

	result = m_SoundClass->Initialize(hwnd, "..//data//Break the Limit!.wav");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the SoundClass.", L"Error", MB_OK);
		return false;
	}

	//오디오 재생
	result = m_SoundClass->PlayAudio();
	if (!result)
	{
		MessageBox(hwnd, L"Could not play the Audio", L"Error", MB_OK);
		return false;
	}
	
	return result;
}

void ApplicationClass::Shutdown()
{

	if (m_SoundClass)
	{
		m_SoundClass->Shutdown();
		delete m_SoundClass;
		m_SoundClass = 0;
	}

	if (m_uiManager)
	{
		m_uiManager->Shutdown();
		delete m_uiManager;
		m_uiManager = 0;
	}

	if (m_TextClass)
	{
		m_TextClass->Shutdown();
		delete m_TextClass;
		m_TextClass = 0;
	}

	if (m_CameraClass)
	{
		delete m_CameraClass;
		m_CameraClass = 0;
	}

	if (m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}
	
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}

bool ApplicationClass::Frame(HWND hwnd, InputClass* pInputClass, FrameTimer* pFrameTimer)
{
	bool result;

	//test
	{
		bool state;
		float fps = pFrameTimer->GetTime();
		float deltaX, deltaY;
		m_CameraClass->SetFrameTime(fps);

		state = pInputClass->GetKeyPressed(DIK_A);
		m_CameraClass->MoveLeft(state);

		state = pInputClass->GetKeyPressed(DIK_S);
		m_CameraClass->MoveBackward(state);

		state = pInputClass->GetKeyPressed(DIK_D);
		m_CameraClass->MoveRight(state);

		state = pInputClass->GetKeyPressed(DIK_W);
		m_CameraClass->MoveForward(state);

		pInputClass->GetMouseDelta(deltaX, deltaY);
		m_CameraClass->UpdateRotation(deltaX, deltaY);
	}

	Render(hwnd, pInputClass);

	return true;
}


void ApplicationClass::Render(HWND hwnd, InputClass* pInputClass)
{
	m_CameraClass->Render();

	//3D RenderTarget 초기화(특정 컬러로)
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.2f, 1.0f);

	//2D RenderTarget 초기화
	m_TextClass->BeginDraw();


	XMMATRIX world, view, proj;

	m_Direct3D->GetWorldMatrix(world);
	m_Direct3D->GetProjectionMatrix(proj);
	m_CameraClass->GetViewMatrix(view);

	//UI 렌더링
	m_uiManager->Frame(m_Direct3D, hwnd, m_ShaderManager, m_TextClass, m_CameraClass, pInputClass);

	m_TextClass->EndDraw();
	m_Direct3D->EndScene();

	return;
}