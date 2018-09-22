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
	// ��Ⱦ��
	_xRender.InitialRender(hWnd);
	// �ű�����
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
	//�е����⣬Ҫ�ȴ���Ⱦ�߳��˳�
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	_xScript.Exit();
}