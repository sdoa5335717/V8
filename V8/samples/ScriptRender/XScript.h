/*
使用V8对解析JSP，并获取参数,传递给D3D Render
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
	// 将渲染器对象和相关接口暴露给JS
	void RegistJSObject(XRender& render, std::string& strObj);

	bool ExecuteScript(const char* srcfile);
	
	void Exit();

	//static void SetVertex(const FunctionCallbackInfo<Value>& info);

private:
	//Local<ObjectTemplate> MakeObjTemplate(Isolate* isolate);
};

