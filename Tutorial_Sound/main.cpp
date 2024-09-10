#include "Systemclass.h"
#pragma comment(linker , "/entry:WinMainCRTStartup /subsystem:console")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	bool result;
	SystemClass* systemClass;

	//SystemClass 생성
	systemClass = new SystemClass;
	if (!systemClass)
	{
		return 0;
	}

	//SystemClass 구현
	result = systemClass->Initialize();
	if (result)
	{
		systemClass->Run();
	}

	//모든 자원 반환
	systemClass->Shutdown();
	delete systemClass;
	systemClass = 0;


	return 0;
}