#include "stdafx.h"
#include "JSFuncDef.h"
#include "log.h"
#include "XRender.h"
#include <assert.h>

// 1���������飬 ��������(��Ϊ��), �Ƿ�ʹ������
void SetRenderParam(const FunctionCallbackInfo<Value>& args)
{
	XRender* pRender = UnWrapRender(args.Holder());
	WriteError("SetRender call Success, Render:0x%08x",  pRender);
	int argc = args.Length();
	if (argc < 1){
		WriteError("invalid paramater\n");
	}
	VertexParamater vParam;
	vParam._bUseIndex = false;
	// ����
	if (args[0]->IsArray())
	{
		v8::Local<v8::Array> arrVertex = v8::Local<v8::Array>::Cast(args[0]);
		for (unsigned int i = 0; i < arrVertex->Length(); i++)
		{
			if (arrVertex->Get(i)->IsArray())
			{
				v8::Local<v8::Array> point = v8::Local<v8::Array>::Cast(arrVertex->Get(i));
				assert(point->Length() == 4);
				vParam._vertexlist.push_back(Vertex(point->Get(0)->NumberValue(),
					point->Get(1)->NumberValue(), point->Get(2)->NumberValue(), point->Get(3)->Uint32Value()));
			}
			else
			{
				WriteError("vertex paramater is invalid\n");
				return;
				//bValidPar = false;
			}
		}
	}
	// ����
	if (argc == 2)
	{
		if (args[1]->IsArray())
		{
			v8::Local<v8::Array> arrIndex = v8::Local<v8::Array>::Cast(args[1]);
			for (unsigned int i = 0; i < arrIndex->Length(); i++)
			{
				if (arrIndex->Get(i)->IsInt32()){
					vParam._indexlist.push_back(arrIndex->Get(i)->Int32Value());
				}
				else
				{
					WriteError("index paramater is invalid\n");
					return;
				}
			}
		}
		else{
			WriteError("index paramater is invalid\n");
		}
		vParam._bUseIndex = true;
	}	
	pRender->AddPrimitive(vParam);
}
// �趨�任����(4*4)
void SetTransform(const FunctionCallbackInfo<Value>& args)
{

}
// ����ƽ�ƾ���
void GetTransformMaxtri(const FunctionCallbackInfo<Value>& args)
{

}
// ������ת����
void GetRotateMaxtri(const FunctionCallbackInfo<Value>& args)
{

}
void GetModule(const FunctionCallbackInfo<Value>& args)
{
	// �����ĵ�
	if (args[0]->IsString())
	{
		v8::HandleScope handle_scope(args.GetIsolate());
		Isolate::Scope isolate_scope(args.GetIsolate());

		v8::String::Utf8Value src(args[0]);

		v8::Local<v8::String> source;
		v8Helper::ReadFile(args.GetIsolate(), *src).ToLocal(&source);

		bool result = v8Helper::ExecuteString(v8Helper::GetIsolate(),
			source, true, true);
	}
}