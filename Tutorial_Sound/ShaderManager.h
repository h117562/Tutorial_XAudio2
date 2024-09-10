#ifndef _SHADER_MANAGER_H_
#define _SHADER_MANAGER_H_

#include "TextureShaderClass.h"
#include "ColorShaderClass.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool Initialize(ID3D11Device*, HWND);
	TextureShaderClass* GetTextureShader();
	ColorShaderClass* GetColorShader();
	void Shutdown();

private:
	TextureShaderClass* m_TextureShader;
	ColorShaderClass* m_ColorShader;

};

#endif