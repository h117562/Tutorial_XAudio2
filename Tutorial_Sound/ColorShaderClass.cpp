#include "ColorShaderClass.h"

ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;

	bufferCount = 1;
	startNumber = 0;
}

ColorShaderClass::~ColorShaderClass()
{
}


bool ColorShaderClass::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	const wchar_t* vsFilename = L"../ColorVs.hlsl";
	const wchar_t* psFilename = L"../ColorPs.hlsl";

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "main", "vs_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);

	if (FAILED(result))
	{
		//파일로부터 컴파일에 실패했을 경우 오류 메시지를 파일로 생성
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//만약 오류 메시지가 없을 경우 파일을 찾을 수 없다는 의미
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vs Texture Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "main", "ps_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

	if (FAILED(result))
	{
		//파일로부터 컴파일에 실패했을 경우 오류 메시지를 파일로 생성
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//만약 오류 메시지가 없을 경우 파일을 찾을 수 없다는 의미
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Ps Texture Shader File", MB_OK);
		}

		return false;
	}

	//정점 쉐이더 생성
	result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//픽셀 쉐이더 생성
	result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//쉐이더의 입력 데이터를 정의
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;	//D3D11_APPEND_ALIGNED_ELEMENT 이랑 같음 기본값
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//입력 데이터의 개수
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//정점 입력 레이아웃 생성 (픽셀 쉐이더는 불가능)
	result = pDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//정점, 픽셀 쉐이더 버퍼 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//매트릭스 버퍼 설정
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//매트릭스 버퍼 생성
	result = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* pDeviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;

	result = UpdateShaderBuffers(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//인풋 어셈블러에서 버퍼를 활성화하여 렌더링 할 수 있도록 설정
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetInputLayout(m_layout);
	pDeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pixelShader, NULL, 0);

	return true;
}


bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, CONST WCHAR* vsFilename, CONST WCHAR* psFilename)
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "main", "vs_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//파일로부터 컴파일에 실패했을 경우 오류 메시지를 파일로 생성
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//만약 오류 메시지가 없을 경우 파일을 찾을 수 없다는 의미
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "main", "ps_4_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//파일로부터 컴파일에 실패했을 경우 오류 메시지를 파일로 생성
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd);
		}
		//만약 오류 메시지가 없을 경우 파일을 찾을 수 없다는 의미
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//정점 쉐이더 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//픽셀 쉐이더 생성
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//쉐이더의 입력 데이터를 정의
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//입력 데이터의 개수
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//정점 입력 레이아웃 생성 (픽셀 쉐이더는 불가능)
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//정점, 픽셀 쉐이더 버퍼 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//상수 버퍼 설정
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//상수 버퍼 생성
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

//에러 내용을 메시지 박스로 출력
void ColorShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd)
{
	char* compileErrors;

	//에러 메시지 텍스트 버퍼에 대한 포인터를 가져옴
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	std::string tempMessage(compileErrors);
	std::wstring widestr = std::wstring(tempMessage.begin(), tempMessage.end());
	const wchar_t* convertMessage = widestr.c_str();

	//에러 메시지를 해제
	errorMessage->Release();
	errorMessage = 0;

	//메시지 박스 띄우기
	MessageBox(hwnd, convertMessage, L"Error", MB_OK);

	return;
}

//쉐이더에서 사용하는 버퍼를 업데이트
bool ColorShaderClass::UpdateShaderBuffers(ID3D11DeviceContext* pDeviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr;

	//행렬 전치
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//매트릭스 버퍼를 매핑 (GPU 에서 데이터를 가져옴) 
	result = pDeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBuffer*)mappedResource.pData;

	//매개변수로 가져온 데이터로 업데이트
	dataPtr->mWorld = worldMatrix;
	dataPtr->mView = viewMatrix;
	dataPtr->mProjection = projectionMatrix;

	//매트릭스 버퍼를 언매핑 (GPU 로 데이터를 전달)
	pDeviceContext->Unmap(m_matrixBuffer, 0);

	pDeviceContext->VSSetConstantBuffers(startNumber, bufferCount, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::Shutdown()
{
	//매트릭스 버퍼 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//레이아웃 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//픽셀 쉐이더 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//정점 쉐이더 해제
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}