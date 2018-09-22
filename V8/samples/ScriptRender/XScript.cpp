#include "stdafx.h"
#include "XScript.h"
#include "XRender.h"
#include "JSFuncDef.h"
#include <assert.h>
using namespace std;
XScript::XScript()
{
}


XScript::~XScript()
{
}

XRender* UnWrapRender(Local<Object> obj)
{
	Local<External> field = Local<External>::Cast(obj->GetInternalField(0));
	void* ptr = field->Value();
	return static_cast<XRender*>(ptr);
}

Local<Object> WrapRender(XRender* obj)
{
	// Local scope for temporary handles.
	EscapableHandleScope handle_scope(v8Helper::GetIsolate());

	Global<ObjectTemplate>* pGlobalTemplete = v8Helper::getGlobalTemplete();
	
	assert(pGlobalTemplete);

	// Fetch the template for creating JavaScript map wrappers.
	// It only has to be created once, which we do on demand.
	if (pGlobalTemplete->IsEmpty()) {
		Local<ObjectTemplate> raw_template = v8Helper::MakeObjTemplate(v8Helper::GetIsolate());
		pGlobalTemplete->Reset(v8Helper::GetIsolate(), raw_template);
	}
	Local<ObjectTemplate> templ =
		Local<ObjectTemplate>::New(v8Helper::GetIsolate(), *pGlobalTemplete);

	// Create an empty map wrapper.
	Local<Object> result =
		templ->NewInstance(v8Helper::GetIsolate()->GetCurrentContext()).ToLocalChecked();

	// Wrap the raw C++ pointer in an External so it can be referenced
	// from within JavaScript.
	Local<External> render_ptr = External::New(v8Helper::GetIsolate(), obj);

	// Store the map pointer in the JavaScript wrapper.
	result->SetInternalField(0, render_ptr);

	// Return the result through the current handle scope.  Since each
	// of these handles will go away when the handle scope is deleted
	// we need to call Close to let one, the result, escape into the
	// outer handle scope.
	return handle_scope.Escape(result);
}

bool XScript::Initlize()
{
	v8Helper::Initialize();

	return true;
}
void XScript::Exit()
{
	v8Helper::ShutDown();
}

void XScript::RegistJSObject(XRender& render, string& strObj)
{
	// Create a handle scope to hold the temporary references.
	HandleScope handle_scope(v8Helper::GetIsolate());
	Local<ObjectTemplate> global = ObjectTemplate::New(v8Helper::GetIsolate());

	//Global<ObjectTemplate>* pGlobal = v8Helper::getGlobalTemplete();

	Global<Context>* pGlobalContext = v8Helper::getGlobalContext();
	assert(pGlobalContext);

	v8::Local<v8::Context> context = Context::New(v8Helper::GetIsolate(), NULL, global);
	pGlobalContext->Reset(v8Helper::GetIsolate(), context);

	//v8::Local<v8::Context> context =
	//	v8::Local<v8::Context>::New(v8Helper::GetIsolate(), *pGlobalContext);
	//render_obj->
	// °ü×°C++ OBJ
	Context::Scope context_scope(context);

	Local<Object> render_obj = WrapRender(&render);

	// bind method
	int nSize = sizeof(g_funTable) / sizeof(fun_templete);

	for (int i = 0; i < nSize; i++)
	{
		render_obj->Set(v8::String::NewFromUtf8(v8Helper::GetIsolate(), g_funTable[i]._strJsFunc.c_str(), NewStringType::kNormal).ToLocalChecked(),
			FunctionTemplate::New(v8Helper::GetIsolate(), g_funTable[i]._pCallback)->GetFunction());
	}

	nSize = sizeof(g_LocalFunc) / sizeof(fun_templete);
	for (int i = 0; i < nSize; i++)
	{
		
		context->Global()
			->Set(context,
			String::NewFromUtf8(v8Helper::GetIsolate(), g_LocalFunc[i]._strJsFunc.c_str(), NewStringType::kNormal)
			.ToLocalChecked(),
			v8::FunctionTemplate::New(v8Helper::GetIsolate(), g_LocalFunc[i]._pCallback)->GetFunction())
			.FromJust();

		//global->Set(v8::String::NewFromUtf8(
		//	v8Helper::GetIsolate(), g_LocalFunc[i]._strJsFunc.c_str(), v8::NewStringType::kNormal).ToLocalChecked(),
		//	v8::FunctionTemplate::New(v8Helper::GetIsolate(), g_LocalFunc[i]._pCallback)->GetFunction());
	}

	context->Global()
		->Set(context,
		String::NewFromUtf8(v8Helper::GetIsolate(), strObj.c_str(), NewStringType::kNormal)
		.ToLocalChecked(),
		render_obj)
		.FromJust();

	// Regist local func 

}
bool XScript::ExecuteScript(const char* srcfile)
{
	if (!srcfile)
	{
		return false;
	}
	HandleScope scope(v8Helper::GetIsolate());
	Context::Scope context(v8Helper::getGlobalContext()->Get(v8Helper::GetIsolate()));
	//Local<Context> context = Local<Context>::New(v8Helper::GetIsolate(), v8Helper::getGlobalContext());
	//Context::Scope context_scope();

	v8::Local<v8::String> source;
	v8Helper::ReadFile(v8Helper::GetIsolate(), srcfile).ToLocal(&source);

	bool result = v8Helper::ExecuteString(v8Helper::GetIsolate(),
		source, true, true);

	return result;
}