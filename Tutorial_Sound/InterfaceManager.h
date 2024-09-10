#ifndef _INTERFACE_MANAGER_H_
#define _INTERFACE_MANAGER_H_

#include "DebugInfoUI.h"
#include "ShaderManager.h"
#include "InputClass.h"

class InterfaceManager
{
public:
	InterfaceManager();
	InterfaceManager(const InterfaceManager&);
	~InterfaceManager();

	bool Initialize(D3DClass*);
	void Frame(D3DClass*, HWND hwnd, ShaderManager*, TextClass*, CameraClass* ,InputClass*);
	void Shutdown();

private:
	DebugInfoUI* m_debugUI;

	bool m_enableDebugUI;
};

#endif