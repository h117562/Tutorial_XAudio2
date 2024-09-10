#include "textclass.h"

TextClass::TextClass()
{
	m_d2dFactory = 0;
	m_dwFactory = 0;
	m_defaultFormat = 0;
	m_renderTarget = 0;
	m_defaultBrush = 0;
}

TextClass::~TextClass()
{

}

bool TextClass::Initialize(IDXGISwapChain* pSwapChain)
{
	HRESULT result;
	IDXGISurface* backBuffer;
	D2D1_RENDER_TARGET_PROPERTIES props;

	//D3D의 스왑체인 버퍼를 가져옴
	result = pSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	if (FAILED(result))
	{
		return false;
	}

	//변수 초기화
	ZeroMemory(&props, sizeof(D2D1_RENDER_TARGET_PROPERTIES));

	//Direct2D 렌더 타켓 설정
	props.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
	props.dpiX = 0.0f;//0 기본
	props.dpiY = 0.0f;//0 기본
	props.usage = D2D1_RENDER_TARGET_USAGE_NONE;
	props.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
	props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	//D2D 팩토리 생성 (싱글 스레드)
	result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2dFactory);
	if (FAILED(result))
	{
		return false;
	}

	//렌더 타켓 생성
	result = m_d2dFactory->CreateDxgiSurfaceRenderTarget(
		backBuffer,
		&props,
		&m_renderTarget
	);

	if (FAILED(result))
	{
		return false;
	}

	//백 버퍼 포인터 해제
	backBuffer->Release();
	backBuffer = 0;

	//Dwrite 팩토리 생성
	result = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,	//해당 팩토리를 공유할지 격리할지 정하는 플래그
		__uuidof(IDWriteFactory),	//팩토리 인터페이스를 식별하는 GUID
		reinterpret_cast<IUnknown**>(&m_dwFactory)//팩토리에 대한 포인터 주소
	);

	if (FAILED(result))
	{
		return false;
	}

	//폰트 설정
	result = m_dwFactory->CreateTextFormat(
		L"나눔스퀘어 네오 OTF",	//폰트 이름
		NULL,					//폰트 컬렉션에 대한 포인터 주소 NULL은 시스템 폰트 컬렉션
		DWRITE_FONT_WEIGHT::DWRITE_FONT_WEIGHT_NORMAL,//폰트 두께
		DWRITE_FONT_STYLE::DWRITE_FONT_STYLE_NORMAL,//폰트 스타일
		DWRITE_FONT_STRETCH::DWRITE_FONT_STRETCH_NORMAL,//폰트 스트레치
		30.0f,					//폰트 사이즈
		L"ko",					//지역 이름 ex) KO, EN
		&m_defaultFormat		//텍스트 형식(IDWriteTextFormat)에 대한 포인터 주소를 반환한다
	);

	if (FAILED(result))
	{
		return false;
	}

	//기본 브러쉬 설정 (빨강)
	result = m_renderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF(1.0f, 0.0f, 0.0f, 1.0f)),
		&m_defaultBrush
	);
	if (FAILED(result))
	{
		return false;
	}

	//기본 텍스트 정렬 (왼쪽)
	result = m_defaultFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

//텍스트 랜더링 기본 폰트, 컬러
void TextClass::RenderText(CONST WCHAR* ptext, FLOAT x, FLOAT y, FLOAT width, FLOAT height)
{
	m_renderTarget->DrawTextW(
		ptext,
		wcslen(ptext),
		m_defaultFormat,
		D2D1::RectF(x, y, width, height),
		m_defaultBrush,
		D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

//텍스트 랜더링 지정 폰트, 컬러
void TextClass::RenderText(CONST WCHAR* ptext, FLOAT x, FLOAT y, FLOAT width, FLOAT height, IDWriteTextFormat* pformat, ID2D1SolidColorBrush* pbrush)
{
	m_renderTarget->DrawTextW(
		ptext,
		wcslen(ptext),
		pformat,
		D2D1::RectF(x, y, width, height),
		pbrush,
		D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

//텍스트 랜더링 시작 함수
void TextClass::BeginDraw()
{
	m_renderTarget->BeginDraw();
}

//텍스트 랜더링 종료 함수
void TextClass::EndDraw()
{
	m_renderTarget->EndDraw();
}

void TextClass::Shutdown()
{
	if (m_defaultBrush)
	{
		m_defaultBrush->Release();
		m_defaultBrush = 0;
	}

	if (m_defaultFormat)
	{
		m_defaultFormat->Release();
		m_defaultFormat = 0;
	}

	if (m_renderTarget)
	{
		m_renderTarget->Release();
		m_renderTarget = 0;
	}

	if (m_dwFactory)
	{
		m_dwFactory->Release();
		m_dwFactory = 0;
	}

	if (m_d2dFactory)
	{
		m_d2dFactory->Release();
		m_d2dFactory = 0;
	}

}