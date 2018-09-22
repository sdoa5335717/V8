#include "stdafx.h"
#include "XRender.h"


#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

const bool bWindowed = true;

void renderThread(void* param)
{
	if (param)
	{
		XRender* pRender = (XRender*)param;
		pRender->Run();
	}
}
XRender::XRender()
{
}
XRender::~XRender()
{
	SAFE_RELEASE(_device);
}
bool XRender::IsReady()
{
	return _device != NULL;
}
bool XRender::InitialRender(HWND hWnd)
{
	HRESULT hr = 0;
	// Step 1: Create the IDirect3D9 object.

	IDirect3D9* d3d9 = 0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d9)
	{
		::MessageBox(0, "Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	// Step 2: Check for hardware vp.

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	RECT rc;
	GetWindowRect(hWnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	// Step 3: Fill out the D3DPRESENT_PARAMETERS structure.
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = bWindowed;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Step 4: Create the device.

	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, // primary adapter
		D3DDEVTYPE_HAL,         // device type
		hWnd,               // window associated with device
		vp,                 // vertex processing
		&d3dpp,             // present parameters
		&_device);            // return created device

	if (FAILED(hr))
	{
		// try again using a 16-bit depth buffer
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			vp,
			&d3dpp,
			&_device);

		if (FAILED(hr))
		{
			d3d9->Release(); // done with d3d9 object
			::MessageBox(0, "CreateDevice() - FAILED", 0, 0);
			return false;
		}
	}

	d3d9->Release(); // done with d3d9 object

	// 投影矩阵设定
	//D3DXVECTOR3 position(0.0f, 0.0f, -15.0f);
	//D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	//D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	//D3DXMATRIX V;
	//D3DXMatrixLookAtLH(&V, &position, &target, &up);

	//_device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set the projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixOrthoLH(&proj,
		(float)width,
		(float)height,
		0.0,
		1.0f
		);
	_device->SetTransform(D3DTS_PROJECTION, &proj);

	D3DXMATRIX Identity;
	D3DXMatrixIdentity(&Identity);

	
	_device->SetTransform(D3DTS_WORLD, &Identity);
	_device->SetTransform(D3DTS_VIEW, &Identity);
	
	
	//
	// Switch to wireframe mode.
	//
	_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	_device->SetRenderState(D3DRS_LIGHTING, false);
	return true;
}
void XRender::CreateVertexBuffer(int nSize)
{
	//_rmutex.lock();
	SAFE_RELEASE(_VB);
	_device->CreateVertexBuffer(
		nSize * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&_VB,
		0);
	//_rmutex.unlock();
}
void XRender::CreateIndexBuffer(int nSize)
{
	//_rmutex.lock();
	SAFE_RELEASE(_IB);
	_device->CreateIndexBuffer(
		nSize * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_IB,
		0);
	//_rmutex.unlock();
}
void XRender::AddPrimitive(const VertexParamater &verparam)
{
	_rmutex.lock();
	_vtParam.Append(verparam);

	_mesh.CreateMesh(_vtParam._indexlist.size() / 3, D3DXMESH_MANAGED, Vertex::FVF,
		_vtParam._vertexlist, _vtParam._indexlist, _device);
	
	_mesh.SetSubset(0, _vtParam._indexlist.size() / 3 - 1, 0);

	_mesh.SetSubset(_vtParam._indexlist.size() / 3 - 1, _vtParam._indexlist.size() / 3 , 1);

	//_vtParam._bUseIndex = verparam._bUseIndex;
	//CreateVertexBuffer(_vtParam._vertexlist.size());
	//// define unique vertices:
	//Vertex* vertices;
	//_VB->Lock(0, 0, (void**)&vertices, 0);
	//vertices[0] = Vertex(-100, 0, 0);/*_vtParam._vertexlist[0]*/;
	//vertices[1] = Vertex(100, 0, 0); //_vtParam._vertexlist[1];
	//vertices[2] = Vertex(0, 100, 0); //_vtParam._vertexlist[2];
	////memcpy_s(vertices, _vtParam._vertexlist.size()*sizeof(Vertex),
	////	_vtParam._vertexlist.data(), _vtParam._vertexlist.size()*sizeof(Vertex));
	//_VB->Unlock();

	//if (_vtParam._bUseIndex)
	//{
	//	CreateIndexBuffer(_vtParam._indexlist.size());
	//	// define the triangles of the cube:
	//	WORD* indices = 0;
	//	_IB->Lock(0, 0, (void**)&indices, 0);
	//	memcpy_s(indices, _vtParam._indexlist.size()*sizeof(WORD),
	//		_vtParam._indexlist.data(), _vtParam._vertexlist.size()*sizeof(WORD));
	//	_IB->Unlock();
	//}
	_rmutex.unlock();
}
void XRender::Start()
{
	//_renderThread = new std::thread(renderThread, this);
	std::thread tRender(renderThread, this);
	tRender.detach();
}
void XRender::Stop()
{
	_isExit = true;
}

void XRender::Run()
{
	while (!_isExit)
	{
		_rmutex.lock();
		//if (!_VB)
		//{
		//	_rmutex.unlock();
		//	std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//	continue;
		//}
		Render();
		_rmutex.unlock();
	}
}

void XRender::Render()
{
	//
	// draw the scene:
	//

	HRESULT hr = S_OK;
	_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
	_device->BeginScene();

	_mesh.DrawSubset();
	//hr = _device->SetStreamSource(0, _VB, 0, sizeof(Vertex));

	//hr = _device->SetFVF(Vertex::FVF);

	//int nVertexCount = _vtParam._vertexlist.size();
	//int nPrimitiveCount = _vtParam._vertexlist.size() / 3;
	//// Draw cube.
	//if (_vtParam._bUseIndex)
	//{
	//	_device->SetIndices(_IB);
	//	_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertexCount, 0, nPrimitiveCount);
	//}
	//else
	//{
	//	hr = _device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nPrimitiveCount);
	//}

	_device->EndScene();
	_device->Present(0, 0, 0, 0);
}