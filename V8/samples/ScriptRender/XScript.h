/*
ʹ��V8�Խ���JSP������ȡ����,���ݸ�D3D Render
*/
#pragma once
#include <string>
#include "v8Helper.h"
class XRender;

extern XRender* UnWrapRender(Local<Object> obj);
extern Local<Object> WrapRender(XRender* obj);

class XScript
{
public:
	XScript();
	virtual ~XScript();

	bool Initlize();
	// ����Ⱦ���������ؽӿڱ�¶��JS
	void RegistJSObject(XRender& render, std::string& strObj);

	bool ExecuteScript(const char* srcfile);
	
	void Exit();

	//static void SetVertex(const FunctionCallbackInfo<Value>& info);

private:
	//Local<ObjectTemplate> MakeObjTemplate(Isolate* isolate);
};

