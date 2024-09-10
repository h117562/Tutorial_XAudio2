#ifndef _GRAPHICS_STRUCTURES_H_
#define _GRAPHICS_STRUCTURES_H_

#include <directxmath.h>

struct VertexTextureCoord
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoord;
};

struct VertexColor
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

struct MatrixBuffer 
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

#endif
