#ifndef _COLOR_SHADER_CLASS_H_
#define _COLOR_SHADER_CLASS_H_

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <string>

#include "GraphicsStructures.h"

using namespace DirectX;

class ColorShaderClass 
{
public:
	ColorShaderClass();
	~ColorShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	bool InitializeShader(ID3D11Device*, HWND, CONST WCHAR*, CONST WCHAR*);
	void OutputShaderErrorMessage(ID3DBlob*, HWND);

	bool UpdateShaderBuffers(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

	UINT bufferCount;
	UINT startNumber;
};

#endif