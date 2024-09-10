#include "d3dclass.h"


D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_rasterStateNoCulling = 0;
	m_rasterStateWireframe = 0;
	m_depthDisabledStencilState = 0;
	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}


bool D3DClass::Initialize(bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_BLEND_DESC blendStateDescription;

	//VSync 여부 
	m_vsync_enabled = vsync;

	//새로고침 빈도
	numerator = 60;		//분자
	denominator = 1;	//분모


	//윈도우 창 크기를 구함
	RECT rect;

	GetClientRect(hwnd, &rect);
	m_screenSize.x = static_cast<unsigned int>(rect.right - rect.left);
	m_screenSize.y = static_cast<unsigned int>(rect.bottom - rect.top);

	//그래픽 인터페이스 팩토리 생성
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//그래픽 인터페이스용 어댑터 생성
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//어댑터 출력(Output)을 열거(Enumerate)함(나열하다)
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//어댑터 출력에서 DXGI_FORMAT_..._UNORM 형식에 맞는 디스플레이 모드 갯수만 가져옴
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//현 컴퓨터의 모든 디스플레이 모드 리스트를 생성
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//아까의 작업을 반복하여 이번엔 디스플레이 모드 리스트의 Structure를 채워줌
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//모든 디스플레이 모드를 확인하여 현재 화면 크기에 맞는 모드를 찾아, 해당 디스플레이의 새로고침 빈도를 가져옴
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == m_screenSize.x)
		{
			if (displayModeList[i].Height == m_screenSize.y)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}


	//그래픽 카드 정보를 가져옴
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//그래픽 카드 메모리를 저장함
	m_videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//그래픽 카드 이름 저장
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//그래픽카드에서 필요한 정보는 다 얻었으니 자원 반환해줌
	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;


	//변수 초기화
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//스왑 체인 설정
	swapChainDesc.BufferCount = 1;						//백 버퍼 갯수
	swapChainDesc.BufferDesc.Width = m_screenSize.x;		//백 버퍼 너비
	swapChainDesc.BufferDesc.Height = m_screenSize.y;		//백 버퍼 높이
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//백 버퍼 형식

	//새로고침 빈도 설정
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//백 버퍼 사용 방식 설정
	swapChainDesc.OutputWindow = hwnd;	//출력할 창의 Hwnd 핸들 
	swapChainDesc.SampleDesc.Count = 1;	//샘플 갯수 설정(1은 다중 샘플링을 끔) ex)슈퍼 샘플링, 멀티 샘플링
	swapChainDesc.SampleDesc.Quality = 0;//품질 레벨 설정(0은 다중 샘플링을 끈 것)

	//전체화면 설정
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//Direct Device 생성
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	//백 버퍼 포인터 가져오기
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//랜더 타겟 뷰 생성
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	//백 버퍼 포인터 해제
	backBufferPtr->Release();
	backBufferPtr = 0;

	//변수 초기화
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//깊이 버퍼 설정
	depthBufferDesc.Width = m_screenSize.x;	//너비
	depthBufferDesc.Height = m_screenSize.y;	//높이
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//깊이 버퍼 생성
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//변수 초기화
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//깊이, 스텐실 설정
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//앞면 픽셀의 경우 작업할 스텐실
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//뒷면 픽셀의 경우 작업할 스텐실
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//깊이 스텐실 State 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	//바로 생성한 State를 적용
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	//변수 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//깊이 스텐실 뷰 설정
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//깊이 스텐실 뷰 생성
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	//렌더 타겟 뷰와 깊이 스텐실 뷰를 출력 렌더 파이프 라인에 바인딩
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	
	//기본 레스터라이저 설정 
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;//폴리곤 앞면만 보여지게 그림 (뒷면을 제거한다는 의미)
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//레스터라이저 상태 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	//설정한 레스터 상태 적용
	m_deviceContext->RSSetState(m_rasterState);

	//와이어 프레임 모드 설정
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//와이어 프레임 모드 상태 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterStateWireframe);
	if (FAILED(result))
	{
		return false;
	}

	//DirectX가 클립 공간 좌표를 렌더링 대상 공간에 매핑할 수 있도록 뷰포트를 설정함
	viewport.Width = static_cast<float>(m_screenSize.x);
	viewport.Height = static_cast<float>(m_screenSize.y);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//뷰포트 생성
	m_deviceContext->RSSetViewports(1, &viewport);

	//Projection 행렬 설정
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = static_cast<float>(m_screenSize.x) / static_cast<float>(m_screenSize.y);

	//렌더링 하기 위한 Perspective Projection 행렬 생성
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	//월드 행렬 초기화
	m_worldMatrix = XMMatrixIdentity();

	//2D 렌더링을 위해 원근감을 없앤 Orthographic Projection(직교 투영) 행렬 생성
	m_orthoMatrix = XMMatrixOrthographicLH(static_cast<float>(m_screenSize.x), static_cast<float>(m_screenSize.y), screenNear, screenDepth);

	//2D 렌더링을 위한 깊이 스텐실 설정
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//2D용 깊이 스텐실 상태 생성
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if (FAILED(result))
	{
		return false;
	}

	//변수 초기화
	ZeroMemory(&blendStateDescription, sizeof(blendStateDescription));

	//블랜드 상태 설정(알파 블랜딩)
	blendStateDescription.AlphaToCoverageEnable = false;
	blendStateDescription.IndependentBlendEnable = false;
	blendStateDescription.RenderTarget[0].BlendEnable = true;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;		
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;				//SourceBlend 현재 픽셀에 대한 블랜드
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;		//DestinationBlend 이전에 그려진 픽셀들에 대한 블랜드
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	


	//알파 블랜딩 상태 생성
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	//모든 블랜딩 끈 상태 설정
	blendStateDescription.RenderTarget[0].BlendEnable = false;

	//블랜딩 상태 생성
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void D3DClass::Shutdown()
{
	//종료하기 전 창모드로 설정하거나 스왑 체인을 해제하면 예외 발생
	//if (m_swapChain)
	//{
	//	m_swapChain->SetFullscreenState(false, NULL);
	//}

	if (m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}

	if (m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if (m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if (m_rasterStateWireframe)
	{
		m_rasterStateWireframe->Release();
		m_rasterStateWireframe = 0;
	}

	if (m_rasterStateNoCulling)
	{
		m_rasterStateNoCulling->Release();
		m_rasterStateNoCulling = 0;
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	//버퍼를 채울 색상을 설정
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//백 버퍼 닦기
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//깊이 버퍼 닦기
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	//백 버퍼를 화면에 표시
	if (m_vsync_enabled)
	{
		//화면 새로 고침 빈도 고정
		m_swapChain->Present(1, 0);
	}
	else
	{
		//준비 되는대로 화면에 최대한 빨리 표시
		m_swapChain->Present(0, 0);
	}

	return;
}


ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

IDXGISwapChain* D3DClass::GetSwapChain()
{
	return m_swapChain;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}


void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DClass::GetScreenSize(XMFLOAT2& screenSize)
{
	screenSize = m_screenSize;
	return;
}


void D3DClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}


void D3DClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}


void D3DClass::EnableAlphaBlending()
{
	float blendFactor[4];

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}


void D3DClass::DisableAlphaBlending()
{
	float blendFactor[4];

	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}


void D3DClass::TurnOnCulling()
{
	//컬링 모드 적용
	m_deviceContext->RSSetState(m_rasterState);

	return;
}


void D3DClass::TurnOffCulling()
{
	//컬링 모드 해제
	m_deviceContext->RSSetState(m_rasterStateNoCulling);

	return;
}


void D3DClass::EnableWireframe()
{
	//와이어 프레임 모드 적용
	m_deviceContext->RSSetState(m_rasterStateWireframe);

	return;
}


void D3DClass::DisableWireframe()
{
	//와이어 프레임 모드 해제
	m_deviceContext->RSSetState(m_rasterState);

	return;
}