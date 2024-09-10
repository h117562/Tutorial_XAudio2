#include "ShaderManager.h"


ShaderManager::ShaderManager()
{
	m_TextureShader = 0;
	m_ColorShader = 0;
}


ShaderManager::~ShaderManager()
{

}



bool ShaderManager::Initialize(ID3D11Device* pDevice, HWND hwnd)
{
	bool result;

	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	result = m_ColorShader->Initialize(pDevice, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ColorShader.", L"Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	result = m_TextureShader->Initialize(pDevice, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextureShader.", L"Error", MB_OK);
		return false;
	}


	return true;
}

TextureShaderClass* ShaderManager::GetTextureShader()
{
	return m_TextureShader;
}


ColorShaderClass* ShaderManager::GetColorShader()
{
	return m_ColorShader;
}


void ShaderManager::Shutdown()
{

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

}