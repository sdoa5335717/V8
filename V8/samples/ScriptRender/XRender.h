#pragma once
#include "v8.h"
#include <vector>
#include <mutex>
#include <thread>
#include "XMesh.h"
struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z, D3DCOLOR color)
	{
		_x = x;  _y = y;  _z = z;
		_color = color;
	}
	float _x, _y, _z;
	D3DCOLOR _color;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

struct VertexParamater
{
	void Append(const VertexParamater& src){
		_vertexlist.insert(_vertexlist.end(), src._vertexlist.begin(), src._vertexlist.end());
		_indexlist.insert(_indexlist.end(), src._indexlist.begin(), src._indexlist.end());
		_bUseIndex = src._bUseIndex;
	}
	// 顶点列表
	std::vector<Vertex> _vertexlist;
	// 索引列表
	std::vector<WORD>	_indexlist;
	// 
	bool _bUseIndex;
};

class XRender
{
public:
	XRender();
	~XRender();

	bool InitialRender(HWND hWnd);

	//void SetVertex(const std::vector<float>& aVertex);
	void AddPrimitive(const VertexParamater &verlist);

	void Start();

	void Stop();
	
	bool IsReady();

	virtual void Run();

	void CompileShader();

protected:
	void Render(); 

private:
	void CreateVertexBuffer(int nSize);
	void CreateIndexBuffer(int nSize);
	bool _isExit = false;
	std::mutex _rmutex;
	//std::thread* _renderThread;
private:
	// D3D相关
	IDirect3DDevice9* _device = NULL;
	
	VertexParamater _vtParam;

	IDirect3DVertexBuffer9* _VB = 0;
	IDirect3DIndexBuffer9*  _IB = 0;

	XMesh _mesh;

};

