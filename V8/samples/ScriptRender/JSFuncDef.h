#ifndef _JSFUNC_CALLBACK_
#define _JSFUNC_CALLBACK_
#include <string>
#include "v8Helper.h"

typedef void(*JS_CALLBACK)(const FunctionCallbackInfo<Value>& args);
class XRender;

XRender* UnWrapRender(Local<Object> obj);
Local<Object> WrapRender(XRender* obj);

struct fun_templete
{
	fun_templete(const std::string& strFunc, const JS_CALLBACK pCallback)
		: _strJsFunc(strFunc), _pCallback(pCallback)
	{}
	std::string _strJsFunc;
	JS_CALLBACK _pCallback;
};
//=================JS functions=====================
// �趨���Ʋ���
// 1���������飬 ��������(��Ϊ��), �Ƿ�ʹ������
void SetRenderParam(const FunctionCallbackInfo<Value>& args);
// �趨�任����(4*4)
void SetTransform(const FunctionCallbackInfo<Value>& args);
// ����ƽ�ƾ���
void GetTransformMaxtri(const FunctionCallbackInfo<Value>& args);
// ������ת����
void GetRotateMaxtri(const FunctionCallbackInfo<Value>& args);

void GetModule(const FunctionCallbackInfo<Value>& args);
//==================================================
// global function

static fun_templete g_funTable[] =
{
	{ "setRenderParam", SetRenderParam },
	{ "setTransform", SetTransform },
	{ "getTransformMaxtri", GetTransformMaxtri },
	{ "getRotateMaxtri", GetRotateMaxtri }
	
};
static fun_templete g_LocalFunc[] = { { "moudle", GetModule } };

#endif