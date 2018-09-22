#pragma once
#include "XRender.h"
#include "XScript.h"
class XScriptRender
{
public:
	XScriptRender();
	virtual ~XScriptRender();
	// 初始化渲染器和脚本解析器
	bool Initialize(HWND hWnd);

	bool ExecuteScript(const char* src);
	// 运行
	void run();

	// 
	void Exit();

private:
	// 窗口, 用于创建D3D
	HWND _hWnd;

	XRender _xRender;

	XScript _xScript;
};

