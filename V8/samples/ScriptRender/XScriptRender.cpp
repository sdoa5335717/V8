#include "stdafx.h"
#include "XScriptRender.h"
#include <string>

XScriptRender::XScriptRender()
{
}


XScriptRender::~XScriptRender()
{
}

bool XScriptRender::Initialize(HWND hWnd)
{
	// 渲染器
	_xRender.InitialRender(hWnd);
	// 脚本解析
	_xScript.Initlize();
	_xScript.RegistJSObject(_xRender, std::string("render"));

	return true;
}

void XScriptRender::run()
{
	_xRender.Start();
}

bool XScriptRender::ExecuteScript(const char* src)
{
	_xScript.ExecuteScript(src);
	return true;
}

void XScriptRender::Exit()
{	
	_xRender.Stop();
	//有点问题，要等待渲染线程退出
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	_xScript.Exit();
}