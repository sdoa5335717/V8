#pragma once
#include "XRender.h"
#include "XScript.h"
class XScriptRender
{
public:
	XScriptRender();
	virtual ~XScriptRender();
	// ��ʼ����Ⱦ���ͽű�������
	bool Initialize(HWND hWnd);

	bool ExecuteScript(const char* src);
	// ����
	void run();

	// 
	void Exit();

private:
	// ����, ���ڴ���D3D
	HWND _hWnd;

	XRender _xRender;

	XScript _xScript;
};

