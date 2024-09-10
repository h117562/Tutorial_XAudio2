#ifndef _TEXTURE_SHADER_CLASS_H_
#define _TEXTURE_SHADER_CLASS_H_

#include <d3dx11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <string>

#include "GraphicsStructures.h"

using namespace DirectX;

class TextureShaderClass
{
public:
	TextureShaderClass();
	~TextureShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix);

private:
	void OutputShaderErrorMessage(ID3DBlob*, HWND);
	bool UpdateShaderBuffers(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_samplerState;

	UINT bufferCount;
	UINT startNumber;
};

#endif