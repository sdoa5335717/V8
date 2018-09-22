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
// 设定绘制参数
// 1，顶点数组， 索引数组(可为空), 是否使用索引
void SetRenderParam(const FunctionCallbackInfo<Value>& args);
// 设定变换矩阵(4*4)
void SetTransform(const FunctionCallbackInfo<Value>& args);
// 计算平移矩阵
void GetTransformMaxtri(const FunctionCallbackInfo<Value>& args);
// 计算旋转矩阵
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